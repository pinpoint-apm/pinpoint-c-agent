<?php
// ref to https://github.com/swoole/swoole-src
//require_once __DIR__ . '/../vendor/autoload.php';

define('AUTOLOAD_FILE_ALIAS',__DIR__.'/../vendor/autoload.php');
define('AOP_CACHE_DIR',__DIR__.'/Cache/');
define('PLUGINS_DIR',__DIR__.'/Plugins/');
//define('APPLICATION_NAME','152APP-4');
//define('APPLICATION_ID','152-app-4');

require_once __DIR__. '/../vendor/eeliu/php_simple_aop/auto_pinpointed.php';

use example\swoole\HandleRequest;

//Create Server Object，Listening 127.0.0.1:9501
$serv = new Swoole\Server("0.0.0.0", 9501);


$serv->on('Connect', function ($serv, $fd) {
    echo "Client: Connect.\n";
});

$userRequest = new HandleRequest();

$serv->on('Receive', array($userRequest, 'onReceiveTcp'));

$serv->on('Close', function ($serv, $fd) {
    echo "Client: Close.\n";
});

$serv->start();
