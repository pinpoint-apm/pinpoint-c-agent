#!/bin/sh
# Run in an isolated CI runner/container with PHP build tools and PIE installed.
set -eu

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 COMPOSER_PHAR PIE_PHAR" >&2
    exit 2
fi
composer_phar=$1
pie_phar=$2
repository_root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
validation_root=$(mktemp -d)
trap 'rm -rf "$validation_root"' EXIT HUP INT TERM
export PIE_WORKING_DIRECTORY="$validation_root/pie"

php "$composer_phar" validate --working-dir="$repository_root" --no-check-publish
expected_version=$(php -r '
    $metadata = json_decode(file_get_contents($argv[1]), true);
    echo $metadata["version"];
' "$repository_root/composer.json")

# Exercise real VCS discovery, including the legacy tag which Composer ignores.
mkdir "$validation_root/repository"
sh "$repository_root/scripts/package_php.sh" "$validation_root/source.tar.gz"
tar -xzf "$validation_root/source.tar.gz" -C "$validation_root/repository"
(
    cd "$validation_root/repository"
    git init -q
    git add .
    git -c user.name=CI -c user.email=ci@example.invalid commit -qm fixture
    git tag "php/v$expected_version"
    git tag "$expected_version"
)
mkdir "$validation_root/consumer"
php -r '
    $config = array("repositories" => array(
        array("type" => "vcs", "url" => $argv[1]),
        array("packagist.org" => false)
    ));
    file_put_contents($argv[2], json_encode($config));
' "$validation_root/repository" "$validation_root/consumer/composer.json"
php "$composer_phar" show --working-dir="$validation_root/consumer" \
    --all pinpoint-apm/pinpoint-php "$expected_version" --format=json \
    > "$validation_root/discovered.json"
php -r '
    $package = json_decode(file_get_contents($argv[1]), true);
    if ($package["type"] !== "php-ext" || !in_array($argv[2], $package["versions"], true)) {
        fwrite(STDERR, "Composer did not discover the PHP release\n");
        exit(1);
    }
' "$validation_root/discovered.json" "$expected_version"

php "$pie_phar" repository:add vcs "$validation_root/repository"
php "$pie_phar" install "pinpoint-apm/pinpoint-php:$expected_version" \
    --skip-enable-extension --no-interaction --make-parallel-jobs=2
php -n -d extension=pinpoint_php.so -r '
    if (phpversion("pinpoint_php") !== $argv[1]) {
        fwrite(STDERR, "PIE installed an incorrect extension version\n");
        exit(1);
    }
    echo "PIE extension version verified: ", $argv[1], "\n";
' "$expected_version"
