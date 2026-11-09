# PHP extension packages and PIE

The extension supports PHP 7.1 and newer. PHP 7.1–8.2 receive only security
fixes and critical bug fixes; new features are not supported on these versions.
The PHP runtime requirement in `composer.json` and `package.xml` must agree.
PIE itself runs on a newer PHP;
its runtime requirement does not raise the extension's minimum PHP version.

## Validate a source package

With PHP development tools, PECL and a C++ compiler installed, run:

```sh
sh scripts/check_php_packages.sh
```

This generates both the PECL archive and the GitHub release archive, extracts
each into a temporary directory, builds it without `versions.json`, checks
`phpversion('pinpoint_php')`, and runs interceptor and runtime smoke tests.
Database, Redis and HTTP integration tests use the separate service CI job.
The package CI matrix covers PHP 8.2, 8.3, 8.4 and 8.5. Historical releases
(PHP 7.1, 7.4 and 8.0) are no longer included in CI.

To reproduce PIE installation and Composer version discovery in an isolated
runner or container with PHP development tools, Git, libtool and unzip:

```sh
sh scripts/check_php_pie.sh /path/to/composer.phar /path/to/pie.phar
```

This installs the extension into that runner's PHP extension directory, using a
temporary local VCS repository. It does not register or publish to Packagist.

## Release checklist

1. Update the PHP version in `versions.json`, run `scripts/sync_versions.py`,
   and update the central PHP release history in `Changes.md`. Regenerate the
   product histories with `scripts/sync_changes.py`.
2. Pass the PHP package and PIE jobs before manually dispatching `release.yml`.
   The release workflow repeats package checks on PHP 8.3 before upload.
3. The workflow retains `php/vX.Y.Z` for GitHub releases and adds an `X.Y.Z`
   tag at the same commit for Composer. Composer ignores `php/vX.Y.Z` tags.
   Existing tags pointing to another commit cause the workflow to fail;
   publish a new PHP version instead of moving an old tag.
4. Register `pinpoint-apm/ext-php` on Packagist with this repository and
   configure its update hook. Verify the new PHP version is indexed before
   announcing the following installation command:

   ```sh
   pie install pinpoint-apm/ext-php
   ```

The explicit Composer version is synchronized from `versions.json`. It also
prevents unrelated collector-agent tags with different versions from being
imported as PHP releases. The native extension name remains `pinpoint_php`;
the separate `pinpoint-apm/pinpoint-php-aop` package remains the PHP AOP layer.
PECL installation continues to be supported.

PIE source installation is tested on Linux. Windows PIE distribution additionally
requires correctly named prebuilt extension archives; existing Windows build
checks do not by themselves establish Windows PIE installation support.
