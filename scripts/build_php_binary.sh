#!/bin/sh
# Build in a disposable PHP development container or CI runner.
set -eu

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 OUTPUT_DIRECTORY" >&2
    exit 2
fi

repository_root=$(CDPATH='' cd -- "$(dirname -- "$0")/.." && pwd)
mkdir -p "$1"
output_directory=$(CDPATH='' cd -- "$1" && pwd)
build_directory=$(mktemp -d)
trap 'rm -rf "$build_directory"' EXIT HUP INT TERM

version=$(php -r '
    $package = json_decode(file_get_contents($argv[1]), true);
    echo $package["version"];
' "$repository_root/composer.json")
php_version=$(php -r 'echo PHP_MAJOR_VERSION, ".", PHP_MINOR_VERSION;')
thread_safety=$(php -r 'echo PHP_ZTS ? "zts" : "nts";')
debug_suffix=$(php -r 'echo PHP_DEBUG ? "-debug" : "";')

case "$(uname -m)" in
    x86_64|amd64) architecture=x86_64 ;;
    arm64|aarch64) architecture=arm64 ;;
    *) echo "Unsupported architecture" >&2; exit 1 ;;
esac
case "$(uname -s)" in
    Linux)
        operating_system=linux
        libc=glibc
        if ldd --version 2>&1 | grep -q musl; then
            libc=musl
        fi
        ;;
    Darwin)
        operating_system=darwin
        libc=bsdlibc
        ;;
    *) echo "Unsupported operating system" >&2; exit 1 ;;
esac

sh "$repository_root/scripts/package_php.sh" "$build_directory/source.tar.gz"
mkdir "$build_directory/source"
tar -xzf "$build_directory/source.tar.gz" -C "$build_directory/source"
cd "$build_directory/source"
phpize
./configure --enable-pinpoint_php --with-php-config="$(command -v php-config)"
make -j"${BUILD_JOBS:-2}"

if [ "$operating_system" = linux ]; then
    # Libtool inserts dynamic C++ dependencies even with GCC's static flags.
    # Link the compiled PIC objects directly so clean runtimes need only libc.
    find . -type f -path '*/.libs/*.o' -print > "$build_directory/objects.list"
    g++ -shared -pthread -static-libstdc++ -static-libgcc \
        @"$build_directory/objects.list" -o modules/pinpoint_php.so
    if readelf -d modules/pinpoint_php.so | grep -E 'NEEDED.*(libstdc\+\+|libgcc_s)'; then
        echo "Binary still depends on the compiler runtime" >&2
        exit 1
    fi
fi

asset="php_pinpoint_php-${version}_php${php_version}-${architecture}-${operating_system}-${libc}${debug_suffix}-${thread_safety}.tgz"
mkdir "$build_directory/package" "$build_directory/extracted"
cp modules/pinpoint_php.so "$build_directory/package/"
cp LICENSE "$build_directory/package/"
tar -czf "$output_directory/$asset" -C "$build_directory/package" pinpoint_php.so LICENSE

# Validate the archived binary, not only the build output.
tar -xzf "$output_directory/$asset" -C "$build_directory/extracted"
php -n -d "extension=$build_directory/extracted/pinpoint_php.so" -r '
    if (phpversion("pinpoint_php") !== $argv[1]) {
        fwrite(STDERR, "Packaged extension version does not match metadata\n");
        exit(1);
    }
    if (!function_exists("_pinpoint_start_trace")) {
        fwrite(STDERR, "Packaged extension is missing its trace API\n");
        exit(1);
    }
    echo "Verified binary extension version: ", phpversion("pinpoint_php"), "\n";
' "$version"
echo "Packaged $output_directory/$asset"
