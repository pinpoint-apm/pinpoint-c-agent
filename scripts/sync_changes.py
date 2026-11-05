#!/usr/bin/env python3
"""Generate product changelogs and check them against the central history."""

import argparse
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parent.parent
CENTRAL = ROOT / "Changes.md"
PRODUCTS = {
    "PHP": ROOT / "CHANGES-PHP.md",
    "Python": ROOT / "CHANGES-PY.md",
    "Common": ROOT / "common/CHANGES.md",
    "Collector-agent": ROOT / "collector-agent/CHANGES.md",
}
SUMMARY_MARKERS = (
    "<!-- BEGIN GENERATED RELEASE SUMMARY -->",
    "<!-- END GENERATED RELEASE SUMMARY -->",
)
SUMMARY_ROWS = {
    "PHP": ("PHP agent", "php-agent"),
    "Python": ("Python agent", "python-agent"),
    "Common": ("Common library", "common-library"),
    "Collector-agent": ("Collector-agent", "collector-agent"),
}
HEADING = re.compile(
    r"^### (PHP|Python|Common|Collector-agent) \(v?([^,\)]+)(?:, ([^\)]+))?\)\s*$",
    re.MULTILINE,
)


def read_history():
    text = CENTRAL.read_text(encoding="utf-8-sig")
    matches = list(HEADING.finditer(text))
    histories = {name: [] for name in PRODUCTS}
    current_date = ""

    for index, match in enumerate(matches):
        preceding_text = text[:match.start()]
        headings = re.findall(r"(?m)^## (.+?)\s*$", preceding_text)
        current_date = headings[-1] if headings and re.fullmatch(r"\d{4}-\d{2}-\d{2}", headings[-1]) else ""
        end = matches[index + 1].start() if index + 1 < len(matches) else len(text)
        section = text[match.end():end].strip()
        section = re.sub(r"\n##?\s+.*\Z", "", section, flags=re.DOTALL).strip()
        product, version, explicit_date = match.groups()
        date = (explicit_date or current_date).strip()
        histories[product].append((version.strip(), date, section))

    return histories


def render_changelog(product, entries):
    lines = [f"# {product} Change History", ""]
    if product == "PHP":
        lines.extend(
            [
                "PECL package changelog: https://pecl.php.net/package-changelog.php?package=pinpoint_php",
                "",
            ]
        )
    for version, date, body in entries:
        date_suffix = f" {date}" if date else ""
        lines.extend([f"## {version}{date_suffix}", ""])
        if body:
            lines.extend([body, ""])
    return "\n".join(lines).rstrip() + "\n"


def render_release_summary(histories):
    lines = [
        "| Agent | Latest version | Security update (upgrade recommended) | Release history |",
        "| --- | --- | --- | --- |",
    ]
    for product, (label, anchor) in SUMMARY_ROWS.items():
        entries = histories[product]
        if not entries:
            continue

        version, _date, latest_body = entries[0]
        security_notice = re.search(
            r"(?m)^>\s*\*\*Upgrade to (v?\d+(?:\.\d+)+)",
            latest_body,
        )
        if security_notice:
            security_update = (
                f"⚠️ Upgrade to {security_notice.group(1)} as soon as possible "
                f"([details](#{anchor}))"
            )
        else:
            security_update = "—"

        lines.append(
            f"| {label} | {version} | {security_update} "
            f"| [View history](#{anchor}) |"
        )
    return "\n".join(lines)


def update_release_summary(text, histories):
    start_marker, end_marker = SUMMARY_MARKERS
    if text.count(start_marker) != 1 or text.count(end_marker) != 1:
        raise ValueError("Changes.md must contain one generated release summary block")

    start = text.index(start_marker) + len(start_marker)
    end = text.index(end_marker)
    if end < start:
        raise ValueError("Release summary markers are out of order")

    summary = "\n" + render_release_summary(histories) + "\n"
    return text[:start] + summary + text[end:]


def sync_changes(check_only=False):
    histories = read_history()
    stale_files = []

    central_text = CENTRAL.read_text(encoding="utf-8-sig")
    updated_central = update_release_summary(central_text, histories)
    if updated_central != central_text:
        if check_only:
            stale_files.append(Path("Changes.md"))
        else:
            CENTRAL.write_text(updated_central, encoding="utf-8-sig")
            print("Updated Changes.md release summary")

    for product, path in PRODUCTS.items():
        expected = render_changelog(product, histories[product])
        actual = path.read_text(encoding="utf-8") if path.exists() else ""
        if actual == expected:
            continue
        if check_only:
            stale_files.append(path.relative_to(ROOT))
        else:
            path.write_text(expected, encoding="utf-8")
            print(f"Generated {path.relative_to(ROOT)}")

    php_entries = histories["PHP"]
    if php_entries:
        version, date, body = php_entries[0]
        package_path = ROOT / "package.xml"
        package_text = package_path.read_text(encoding="utf-8")
        notes = f"## {version.lstrip('v')} {date}\n" + body
        indented_notes = "\n".join(
            f"    {line}" if line else "" for line in notes.splitlines()
        )
        updated, count = re.subn(
            r"(?s)  <notes>.*?  </notes>",
            "  <notes>\n" + indented_notes + "\n  </notes>",
            package_text,
            count=1,
        )
        if count != 1:
            raise ValueError("Could not find top-level PEAR notes in package.xml")
        if updated != package_text:
            if check_only:
                stale_files.append(Path("package.xml"))
            else:
                package_path.write_text(updated, encoding="utf-8")
                print("Updated package.xml notes")

    if stale_files:
        for path in stale_files:
            print(f"Out of sync: {path}")
        raise SystemExit(1)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    sync_changes(check_only=args.check)


if __name__ == "__main__":
    main()
