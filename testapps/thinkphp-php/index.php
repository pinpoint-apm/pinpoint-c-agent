<?php
// +----------------------------------------------------------------------
// | ThinkPHP [ WE CAN DO IT JUST THINK ]
// +----------------------------------------------------------------------
// | Copyright (c) 2006-2019 http://thinkphp.cn All rights reserved.
// +----------------------------------------------------------------------
// | Licensed ( http://www.apache.org/licenses/LICENSE-2.0 )
// +----------------------------------------------------------------------
// | Author: liu21st <liu21st@gmail.com>
// +----------------------------------------------------------------------

// [ 应用入口文件 ]
namespace think;

require __DIR__ . '/../vendor/autoload.php';

class TpRequestPlugin extends \Pinpoint\Plugins\Tp8PerRequest
{
    public function __construct()
    {
        $blackUri = ['/favicon.ico'];
        // if uri in blackUri, skips it 
        if (!in_array($_SERVER['REQUEST_URI'], $blackUri)) {
            parent::__construct();
        }
    }
}
define('APPLICATION_NAME', 'cd.dev.test.php'); // your application name
define('APPLICATION_ID', 'cd.dev.tp');  // your application id
define('PP_REQ_PLUGINS', TpRequestPlugin::class);
require_once __DIR__ . '/../vendor/pinpoint-apm/pinpoint-php-aop/auto_pinpointed.php';

// 执行HTTP应用并响应
$http = (new App())->http;

$response = $http->run();

$response->send();

$http->end($response);
