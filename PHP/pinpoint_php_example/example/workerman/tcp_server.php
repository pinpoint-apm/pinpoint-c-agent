<?php
//require_once __DIR__ . '/../vendor/autoload.php';

define('AUTOLOAD_FILE_ALIAS',__DIR__.'/../vendor/autoload.php');
define('AOP_CACHE_DIR',__DIR__.'/Cache/');
define('PLUGINS_DIR',__DIR__.'/Plugins/');

require_once __DIR__. '/../vendor/eeliu/php_simple_aop/auto_pinpointed.php';

use Workerman\Worker;
use example\workerman\HandleRequest;

// #### create socket and listen 1234 port ####
$tcp_worker = new Worker("tcp://0.0.0.0:1234");

// 4 processes
$tcp_worker->count = 4;

// Emitted when new connection come
$tcp_worker->onConnect = function($connection)
{
    echo "New Connection\n";
};
$userRequest = new HandleRequest();

// Emitted when data received
$tcp_worker->onMessage = array($userRequest,'onMessage');

//$tcp_worker->onMessage = function($connection, $data)
//{
//    // send data to client
//
//    $connection->send("hello $data \n");
//};

// Emitted when new connection come
$tcp_worker->onClose = function($connection)
{
    echo "Connection closed\n";
};

Worker::runAll();