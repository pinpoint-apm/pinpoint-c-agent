# PHP Change History

PECL package changelog: https://pecl.php.net/package-changelog.php?package=pinpoint_php

## 0.8.0 2026-11-16

- prefer PIE binaries with source fallback; add PHP 8.2–8.4 Linux glibc/musl and macOS x86_64/arm64 NTS/ZTS packages
- extend Windows binary packages to PHP 8.2 and publish numeric PHP releases before binary uploads
- publish PHP package metadata as version 0.8.0, independently of other products
- restore PHP 7.1-compatible object handling and validate source packages on PHP 8.2–8.5 (#774)
- add PIE packaging as `pinpoint-apm/ext-php`, retaining `pinpoint_php` as the extension name
- add Windows PHP 8.3–8.5 TS/NTS package builds and release assets (#774)

## 0.6.2 2026-11-03

- preserve interceptor exception and bailout semantics (#764)
- fix inherited internal method interceptor resolution

## 0.5.3 2024-07-31

- build on win32
- build on macos
- support SendSpanTimeOutMs=-1

## 0.5.2 2024-05-30

- add pinpoint_get_caller_arg

## 0.5.1 2024-05-29

- add _pinpoint_join_cut and test case
- rename api naming
- add pinpoint_get_this api and test case
- remove php5 support

## 0.5.0

- support common library v0.5.0

## 0.5.3 2024-07-31

- build on win32
- build on macos
- support SendSpanTimeOutMs=-1

## 0.5.2 2024-05-30

- add pinpoint_get_caller_arg

## 0.5.1 2024-05-29

- add _pinpoint_join_cut and test case
- rename api naming
- add pinpoint_get_this api and test case
- remove php5 support

## 0.5.0

- support common library v0.5.0

</details>


<details>
<summary id="python-agent">Python agent — latest 1.4.2</summary>
