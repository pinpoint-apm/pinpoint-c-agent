<?php

// comment out the following two lines when deployed to production
defined('YII_DEBUG') or define('YII_DEBUG', true);
defined('YII_ENV') or define('YII_ENV', 'dev');

require __DIR__ . '/../vendor/autoload.php';
require __DIR__ . '/../vendor/yiisoft/yii2/Yii.php';


// A writable path for caching AOP code
define('APPLICATION_NAME', 'cd.dev.test.php'); // your application name
define('APPLICATION_ID', 'cd.dev.test.yii');  // your application id
define('PP_REQ_PLUGINS', \Pinpoint\Plugins\Yii2PerRequestPlugins::class);
require_once __DIR__ . '/../vendor/pinpoint-apm/pinpoint-php-aop/auto_pinpointed.php';


$config = require __DIR__ . '/../config/web.php';

(new yii\web\Application($config))->run();
