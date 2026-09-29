# Changes

## v0.7.9

> ⚠️ **SECURITY**: v0.7.8 (and earlier) contains a remote denial-of-service
> vulnerability in the collector-agent. **Upgrade to v0.7.9 as soon as possible.**

### Security (collector-agent)
- **Fix remote DoS**: an unauthenticated remote attacker could crash the
  collector-agent with a single TCP packet, causing a denial of service for all
  traced applications. Root cause: `ParseDotFormatToTime` accessed `ar[1]` out
  of bounds on attacker-controlled input, and the filter-chain goroutine had no
  `recover()`. Also hardened several related panic-prone paths (annotation
  parsing, nil `span.Follows` elements, `idMap` type-confusion, empty-slice
  index). See `collector-agent/CHANGES.md` for details.

### Build & CI
- Stop committing generated protobuf Go code (regenerated at build time).
- `cpp-windows`: use Visual Studio 18 2026.
- `PHP-Win-2019` → `PHP-Win-2022`; drop PHP 7.x from CI matrix (EOL).

## v0.4.1

### Fix

- tracelimit failed
- compile problem
- **Full and pending**: fork and  clean font-agent
- Big clue  #252
- divide zero 
- cmd channel re-connection
- crash in mark_an_error
- thrift crash when json span in wrong format

### Feat

- memory.usage
- **php 8.0**: Support php 8.0 in module  #249
- **cinder-api**: plugins,python2.7

### Refactor

- Django,Flask plugins

## v0.4.0-beta  API for asynchronus framework: swoole-src(easyswoole),workerman
* [PHP] add `id` to keep the async call chain
* [python] idem

## v0.3.2
* [Python]python2.7 module support
* [PHP] thinkphp6,Codeigniter4,Symfony framework demo
* [PHP] Guzzle, MongoClient support

## v0.3.0-beta

- support Python
  - demo for django
  - demo for flask
  - demo for tornado
  - demo for BaseHTTPRequestHandler
- easier to use
- support manual in CN

## v0.2.3 

- Support Yii framework classloader [(How to use pinpoint-php agent into Yii?)](/Example/PHP/demo/yii-demo/Readme.md)
- Add some out-of-box plugins for Flraum [ (How to use pinpoint-php agent into Flarum?)](/Example/PHP/demo/Flarum-demo/Readme.md)
- Fix bug in reference parameters
- Supporting the return type is object

## v0.2.2

- Support GRPC