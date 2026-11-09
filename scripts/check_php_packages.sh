#!/bin/sh
# Build only extracted package contents, never the repository checkout.
set -eu

repository_root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
validation_root=$(mktemp -d)
trap 'rm -rf "$validation_root"' EXIT HUP INT TERM
expected_version=$(sed -n 's/^#define PHP_PINPOINT_PHP_VERSION "\([^"]*\)"/\1/p' \
    "$repository_root/php_pinpoint_php.h")
if [ -z "$expected_version" ]; then
    echo "Cannot resolve the synchronized PHP version" >&2
    exit 1
fi

build_package() {
    archive=$1
    build_directory=$2
    mkdir -p "$build_directory"
    tar -xzf "$archive" -C "$build_directory"
    if [ ! -f "$build_directory/config.m4" ]; then
        build_directory="$build_directory/pinpoint_php-$expected_version"
    fi
    test -f "$build_directory/config.m4"
    test ! -f "$build_directory/versions.json"
    # PECL packages omit the test suite. Use the same tests for both archives.
    if [ ! -d "$build_directory/tests" ]; then
        cp -R "$repository_root/tests" "$build_directory/tests"
    fi
    (
        cd "$build_directory"
        phpize
        ./configure --enable-pinpoint_php
        make -j"${BUILD_JOBS:-2}"
        php -n -d "extension=$build_directory/modules/pinpoint_php.so" -r '
            $actual = phpversion("pinpoint_php");
            if ($actual !== $argv[1]) {
                fwrite(STDERR, "Version mismatch: $actual != {$argv[1]}\n");
                exit(1);
            }
            echo "Verified packaged extension version: $actual\n";
        ' "$expected_version"
        # Package smoke tests cover exception ownership, bailout and runtime APIs.
        # Database and external HTTP integration tests run in the service CI job.
        NO_INTERACTION=1 REPORT_EXIT_STATUS=1 make test \
            TESTS="--show-diff tests/interceptor_native*.phpt tests/app_info_test.phpt"
    )
}

# PECL reads its manifest relative to the source directory. Package in a copy
# so generated archives and logs never change the checkout.
mkdir "$validation_root/source"
cp -R "$repository_root/common" "$validation_root/source/common"
for source_file in package.xml config.m4 config.w32 pinpoint_php.cpp \
    php_pinpoint_php.h LICENSE CHANGES-PHP.md README.md; do
    cp "$repository_root/$source_file" "$validation_root/source/"
done
(
    cd "$validation_root/source"
    pecl package
)
build_package "$validation_root/source/pinpoint_php-$expected_version.tgz" "$validation_root/pecl"

sh "$repository_root/scripts/package_php.sh" "$validation_root/github.tar.gz"
build_package "$validation_root/github.tar.gz" "$validation_root/github"
