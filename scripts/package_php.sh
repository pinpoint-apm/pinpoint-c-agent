#!/bin/sh
# Keep the GitHub release and CI source archives identical.
set -eu

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 OUTPUT_ARCHIVE" >&2
    exit 2
fi

repository_root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
tar -czf "$1" -C "$repository_root" \
    common pinpoint_php.cpp php_pinpoint_php.h config.m4 config.w32 \
    tests LICENSE CHANGES-PHP.md composer.json README.md
