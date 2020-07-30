<?php
// orgin from https://github.com/walkor/Workerman
require_once __DIR__ . '/../vendor/autoload.php';

//define('AUTOLOAD_FILE_ALIAS',__DIR__.'/../vendor/autoload.php');
define('AOP_CACHE_DIR',__DIR__.'/Cache/');
define('PLUGINS_DIR',__DIR__.'/Plugins/');

require_once __DIR__. '/../vendor/naver/pinpoint-php-aop/auto_pinpointed.php';

use Workerman\Worker;
use example\workerman\HandleRequest;

// #### http worker ####
$http_worker = new Worker("http://0.0.0.0:2345");


// 4 processes
$http_worker->count = 4;

$userRequest = new HandleRequest();

// Emitted when data received
$http_worker->onMessage = array($userRequest,'onMessage');

// run all workers
Worker::runAll();