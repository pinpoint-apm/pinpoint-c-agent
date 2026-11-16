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
The fixture explicitly suppresses binary downloads to verify the source fallback.

## PIE binary packages

The package requests `pre-packaged-binary` before `composer-default`. Unix PIE
downloads a matching binary when available and compiles from source when no
asset matches. Windows continues to use PIE's Windows DLL download method.

| Platform | PHP versions | Architectures | Modes | Build baseline |
| --- | --- | --- | --- | --- |
| Linux glibc | 8.2, 8.3, 8.4 | x86_64, arm64 | NTS, ZTS | Official PHP Debian 12 (bookworm) images |
| Linux musl | 8.2, 8.3, 8.4 | x86_64, arm64 | NTS, ZTS | Official PHP Alpine 3.23 images |
| macOS | 8.2, 8.3, 8.4 | x86_64, arm64 | NTS, ZTS | macOS 15 runners, deployment target 13.0 |
| Windows | 8.2, 8.3, 8.4; existing 8.5 coverage retained | x64 | NTS, TS | Official PHP Windows builder |

`php-unix-release.yml` builds 36 Unix archives on native runners. Each archive
contains `pinpoint_php.so` at its root and the license. The build script extracts
the archive and checks its runtime version and trace API. A second verifier uses
the pinned PIE 1.5.5 classes to check the filename, select the binary without
compiling, and load that selected file. These local checks do not replace a
post-publication `pie install` check against the actual GitHub release.

For example, PHP 8.3 NTS on Linux x86_64 with glibc selects:

```text
php_pinpoint_php-0.8.0_php8.3-x86_64-linux-glibc-nts.tgz
```

macOS filenames use `darwin-bsdlibc`; ZTS filenames end in `-zts.tgz`.
Debug builds and PHP versions outside this matrix use the Unix source fallback.
PIE matches PHP major/minor, architecture, libc, debug mode and thread safety;
its filename matching does not check the installed libc version. Linux glibc
packages require glibc 2.36 or newer; musl packages target Alpine 3.23 or newer.
The Linux build links libstdc++ and libgcc statically to reduce runtime library
requirements. macOS packages must also satisfy dependencies reported by `otool -L`;
setting a deployment target alone does not guarantee compatibility with every
older macOS installation.

When a matching binary exists but cannot load on an older operating system,
PIE does not automatically retry a source build. Force source installation on
Unix with:

```sh
pie install pinpoint-apm/ext-php:0.8.0 --suppress-download-url-method=pre-packaged-binary
```

The manual source release workflow creates both `php/vX.Y.Z` and numeric `X.Y.Z`
releases before starting Unix and Windows jobs. Each binary workflow uploads
only after its complete matrix succeeds, including SHA256 checksum lists. Both
releases receive the binary assets; the numeric release is required for PIE.
Source releases can remain available if a binary matrix fails. Retry the
appropriate workflow on the exact released commit with `publish` enabled.
Pull requests and manual runs with `publish` disabled retain Actions artifacts
without uploading release assets.

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

The explicit Composer version is synchronized from `versions.json` and must
match the numeric PHP release tag. Product versions have no fixed major
version restriction. Standard numeric tags are reserved for PHP; Python PyPI
releases use `python/vX.Y.Z` tags, which Composer does not import as stable
PHP versions. Packagist also ignores a numeric tag if its version conflicts
with the `version` field in that tag's `composer.json`. The
collector-agent is published through the container Packages workflow and is
unchanged by this split. The native extension name remains `pinpoint_php`;
the separate `pinpoint-apm/pinpoint-php-aop` package remains the PHP AOP layer.
PECL installation continues to be supported.

Windows package loading checks are documented in [windows-packages.md](windows-packages.md).
