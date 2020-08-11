<?php

// comment out the following two lines when deployed to production
defined('YII_DEBUG') or define('YII_DEBUG', true);
defined('YII_ENV') or define('YII_ENV', 'dev');

require __DIR__ . '/../vendor/autoload.php';
require __DIR__ . '/../vendor/yiisoft/yii2/Yii.php';

$config = require __DIR__ . '/../config/web.php';
///////////////Setting of pinpoint php////////////////////
//Cached class
define('AOP_CACHE_DIR',__DIR__.'/../Cache/');
// where to load pinpoint plugins
define('PLUGINS_DIR',__DIR__.'/../Plugins/');

define('APPLICATION_NAME','APP-yii');
define('APPLICATION_ID','app-yii');
// 
/**
 * unregister Yii class loader
 * wrapper with PinpointYiiClassLoader
 */
function pinpoint_user_class_loader_hook()
{
    $loaders = spl_autoload_functions();
    foreach ($loaders as $loader) {
        if(is_array($loader) && is_string($loader[0]) && $loader[0] =='Yii'){
            spl_autoload_unregister($loader);
            spl_autoload_register(['Plugins\PinpointYiiClassLoader','autoload'],true,false);
            break;
        }
    }
}

pinpoint_user_class_loader_hook();


$app = new yii\web\Application($config);
require_once __DIR__. '/../vendor/naver/pinpoint-php-aop/auto_pinpointed.php';

$app->run();

