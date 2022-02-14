<?php
// +----------------------------------------------------------------------
// | ThinkPHP [ WE CAN DO IT JUST THINK IT ]
// +----------------------------------------------------------------------
// | Copyright (c) 2006-2015 http://thinkphp.cn All rights reserved.
// +----------------------------------------------------------------------
// | Licensed ( http://www.apache.org/licenses/LICENSE-2.0 )
// +----------------------------------------------------------------------
// | Author: yunwuxin <448901948@qq.com>
// +----------------------------------------------------------------------
namespace tests;

require_once __DIR__."/config.php";
use tests\Util;

class TestCase extends \think\testing\TestCase
{
    protected $baseUrl = 'http://localhost';
    protected $util;

    /**
     * @before
     */
    public function beforeTest()
    {
        file_put_contents(PHP_LOG_PATH, "");
        $this->util = new Util();
    }

    /**
     * @after
     */
//    public function clearPhpLog()
//    {
//        file_put_contents(PHP_LOG_PATH, "");
//    }
}