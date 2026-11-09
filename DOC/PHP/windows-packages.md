# Windows PHP extension packages

The manual `release php package` workflow publishes the existing PHP source
release, then calls `PHP Windows packages` to build and test Windows binaries.
The matrix covers PHP 8.3, 8.4 and 8.5, x64, in both TS and NTS modes.

Each build uses `php/php-windows-builder/extension@v1`, runs `tests_win32`, and
extracts its ZIP to verify that the packaged DLL loads and reports the release
version. All six builds must pass before any Windows packages are uploaded.
The workflow reads the synchronized version from `php_pinpoint_php.h`; it does
not read `versions.json` during packaging.

## Releases and file names

Windows ZIPs are attached to the existing `php/vX.Y.Z` source release. They are
also attached to a standard `X.Y.Z` release because PIE looks up GitHub releases
by the Composer package version. The standard release is created from the same
source commit and is not marked as the repository's latest release. Existing
tags pointing to another commit are rejected rather than moved.

For example, a PHP 8.3 NTS package might be named:

```text
php_pinpoint_php-0.6.3-8.3-nts-vs16-x86_64.zip
```

The ZIP contains a DLL with the same base name. The official builder selects the
compiler appropriate for each PHP version and includes license/debug files.

## Build without publishing

Pull requests changing the Windows workflow, its verifier, or extension sources
run the matrix and retain packages as Actions artifacts without publishing a
release. Alternatively, manually run `PHP Windows packages` on the desired
source branch with `publish` left disabled.

For a retry after the PHP source release has been published, run the workflow on
the exact released commit and enable `publish`. The workflow checks that the
extension version, source release tag and checked-out commit agree before
building. It uploads only Windows package ZIPs from that workflow run; build logs
are not attached to releases.

The existing PHP source release may remain available if a Windows build fails.
The failed Windows workflow can be retried independently after resolving the
failure. Published Windows packages require a matching PHP runtime architecture,
compiler and thread-safety mode.
