# Changes

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