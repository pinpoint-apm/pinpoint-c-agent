<?php
// origin from https://github.com/swoole/swoole-src
require_once __DIR__ . '/../vendor/autoload.php';

//define('AUTOLOAD_FILE_ALIAS',__DIR__.'/../vendor/autoload.php');
define('AOP_CACHE_DIR',__DIR__.'/Cache/');
define('PLUGINS_DIR',__DIR__.'/Plugins/');

require_once __DIR__. '/../vendor/eeliu/php_simple_aop/auto_pinpointed.php';

use example\swoole\HandleRequest;

// #### http Server ####
$http = new Swoole\Http\Server("0.0.0.0", 9501);

$userRequest = new HandleRequest();

// Emitted when data received
$http->on('request', array($userRequest, 'onReceiveHttp'));

// run
$http->start();