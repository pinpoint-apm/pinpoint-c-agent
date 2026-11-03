---
name: php-release
description: Prepare and validate a Pinpoint PHP agent PEAR package from the repository version metadata. Use when updating PHP release metadata, running pecl/pear packaging, or changing the PHP CI packaging job.
---

# Pinpoint PHP release

Use this skill for PHP package releases in this repository. Keep the release
version consistent across the JSON source of truth, PEAR metadata, changelog,
and CI artifact.

## Version and release metadata

1. Read `versions.json` first. The `php` value is the intended release version.
2. Update the top-level release metadata in `package.xml` to match it:
   - `<date>` must be the release date.
   - `<version><release>` and `<version><api>` must use the PHP version.
   - The top-level `<notes>` heading must use the same version and date.
3. Derive the notes from the actual PHP changes and the top entry in
   `CHANGES-PHP.md`; do not copy an old release description or invent changes.
4. Preserve the historical entries under `package.xml`'s `<changelog>` unless
   the task explicitly asks to correct release history.

Example for a `php` value of `0.6.2`:

```xml
<version>
  <release>0.6.2</release>
  <api>0.6.2</api>
</version>
```

## Local packaging

Run the package command from the repository root after checking the metadata:

```bash
pecl package
pecl build
```

`pecl package` reads `package.xml`; it does not read `versions.json`. Therefore
do not assume changing `versions.json` alone changes the PEAR filename. The
expected artifact is `pinpoint_php-<php-version>.tgz`.

Do not add or depend on `package_php.sh` for this workflow. The repository uses
the direct PEAR commands in the packaging job.

## CI responsibilities

In `.github/workflows/ci-php.yml`:

- `PHP`, `PHP-MacOS`, and `PHP-Win-2022` are test jobs only.
- `pack_php_module` is the only job that runs `pecl package`/`pecl build` and
  uploads `pinpoint_php*.tgz` as an artifact.
- Keep packaging out of the test jobs; a successful test job must not imply a
  package was published.

## Validation checklist

Before reporting success:

```bash
git diff --check
pecl package
ls -l pinpoint_php-*.tgz
```

Confirm that the generated filename matches `versions.json`, and distinguish
local artifact generation from a remote release upload. Do not delete an
existing artifact unless the user explicitly requests cleanup.
