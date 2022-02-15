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

use think\Exception;
use think\Db;

require_once __DIR__."/config.php";

class AutoTest extends TestCase
{

    public function testGetDate()
    {
        $this->visit('/get_date');
        $this->assertResponseOk();
        $log = $this->util->get_log();
        $this->assertTrue($this->util->check_error($log), $log);
        $this->assertTrue($this->util->check_span($log,["app\common\AppDate::outputDate","date"]), $log);
    }

//     public function testFunc1()
//     {
//         $this->visit('/test_func1');
//         $this->assertResponseOk();
//         $log = $this->util->get_log();
//         $this->assertTrue($this->util->check_error($log), $log);
//         $this->assertTrue($this->util->check_span($log,["app\common\AopFunction::test_func1"]), $log);
//     }

    public function testFunc2Caller()
    {
        $this->visit('/test_func2_caller');
        $this->assertResponseOk();
        $log = $this->util->get_log();
        $this->assertTrue($this->util->check_error($log), $log);
        $this->assertTrue($this->util->check_span($log,["app\common\AopFunction::test_func2", "app\util\Foo::test_func_2"]), $log);
    }

    public function testInherit()
    {
        $this->visit('/test_inherit_func');
        $this->assertResponseOk();
        $log = $this->util->get_log();
        $this->assertTrue($this->util->check_error($log), $log);
        $this->assertTrue($this->util->check_span($log,["app\common\Person::eat", "app\common\Teacher::eat", "app\common\Student::eat", "app\common\Doctor::other"]), $log);    # "app\common\Doctor::eat"
    }

    public function testGenerator()
    {
        $this->visit('/test_generator_func');
        $this->assertResponseOk();
        $log = $this->util->get_log();
        $this->assertTrue($this->util->check_error($log), $log);
        $this->assertTrue($this->util->check_span($log,["app\common\TestGenerator::generator", "next"]), $log);
    }

    public function testAbstract()
    {
        $this->visit('/test_abstract_func');
        $this->assertResponseOk();
        $log = $this->util->get_log();
        $this->assertTrue($this->util->check_error($log), $log);
        $this->assertTrue($this->util->check_span($log,["app\common\AbstractStudent::eat", "app\common\AbstractStudent::drink", "app\common\AbstractPerson::breath"]), $log);   # "app\common\AbstractStudent::breath"
    }

    public function testInterface()
    {
        $this->visit('/test_interface_func');
        $this->assertResponseOk();
        $log = $this->util->get_log();
        $this->assertTrue($this->util->check_error($log), $log);
        $this->assertTrue($this->util->check_span($log,["app\common\InterfaceStudent::setVariable", "app\common\InterfaceStudent::getHtml", "app\common\InterfaceStudent::other"]), $log);
    }

    public function testClone()
    {
        $this->visit('/test_clone');
        $this->assertResponseOk();
        $log = $this->util->get_log();
        $this->assertTrue($this->util->check_error($log), $log);
        $this->assertTrue($this->util->check_span($log,["app\common\TestClone::setColor", "app\common\TestClone::getColor", "app\common\TestClone::__clone"]), $log);
    }

    public function testRecursion()
    {
        $this->visit('/test_recursion');
        $this->assertResponseOk();
        $log = $this->util->get_log();
        $this->assertTrue($this->util->check_error($log), $log);
        $this->assertTrue($this->util->check_span($log,["app\common\TestRecursion::test_cumsum1"]), $log);    #, "app\common\TestRecursion::test_cumsum2"
    }

    public function testAnonymous()
    {
        $this->visit('/test_anonymous');
        $this->assertResponseOk();
        $log = $this->util->get_log();
        $this->assertTrue($this->util->check_error($log), $log);
        $this->assertTrue($this->util->check_span($log,["app\common\TestAnonymous::createbell", "app\common\AopFunction::test_func1"]), $log);
    }

    public function testStatic()
    {
        $this->visit('/test_static');
        $this->assertResponseOk();
        $log = $this->util->get_log();
        $this->assertTrue($this->util->check_error($log), $log);
        $this->assertTrue($this->util->check_span($log,["app\common\TestStatic::static_func"]), $log);
    }

    public function testFinalClass()
    {
        $this->visit('/test_final_class');
        $this->assertResponseOk();
        $log = $this->util->get_log();
        $this->assertTrue($this->util->check_error($log), $log);
        $this->assertTrue($this->util->check_span($log,["app\common\TestFinalClass::test"]), $log);
    }

    public function testFinalFunc()
    {
        $this->visit('/test_final_func');
        $this->assertResponseOk();
        $log = $this->util->get_log();
        $this->assertTrue($this->util->check_error($log), $log);
        $this->assertTrue($this->util->check_span($log,["app\common\TestFinalFunc::test"]), $log);
    }

    public function testTrait()
    {
        $this->visit('/test_trait');
        $this->assertResponseOk();
        $log = $this->util->get_log();
        $this->assertTrue($this->util->check_error($log), $log);
        $this->assertTrue($this->util->check_span($log,["app\common\FuncInTrait::traitfunc", "app\common\TestTrait::test"]), $log);
    }

    public function testCallback()
    {
        $this->visit('/test_callback');
        $this->assertResponseOk();
        $log = $this->util->get_log();
        $this->assertTrue($this->util->check_error($log), $log);
        $this->assertTrue($this->util->check_span($log,["app\common\CallBackFunc::fnCallback1", "app\common\CallBackFunc::fnCallback2"]), $log);
    }

    public function testLevel()
    {
        $this->visit('/test_level');
        $this->assertResponseOk();
        $log = $this->util->get_log();
        $this->assertTrue($this->util->check_error($log), $log);
        $this->assertTrue($this->util->check_span($log,["app\common\LevelClass::public_function", "app\common\LevelClass::protected_function", "app\common\LevelClass::private_function", "app\common\OverRideLevel::public_function", "app\common\OverRideLevel::protected_function"]), $log);
    }

    public function testArgs()
    {
        $this->visit('/test_args');
        $this->assertResponseOk();
        $log = $this->util->get_log();
        $this->assertTrue($this->util->check_error($log), $log);
        $this->assertTrue($this->util->check_span($log,["app\common\TestArgs::test_args","[1] => 123","3.1415","[4] => Array","[5] => app\index\controller\SObject Object", "[6] => app\index\controller\SObjectString Object","[7] => abcd", "[8] => Resource id","[9] => Hello you! How are you today?"]), $log);
    }

    public function testReturn()
    {
        $this->visit('/test_return');
        $this->assertResponseOk();
        $log = $this->util->get_log();
        $this->assertTrue($this->util->check_error($log), $log);
        $this->assertTrue($this->util->check_span($log,["app\common\TestReturn::test_return","[0] =>", "[0] => 123", "[0] => 3.1415", "[0] => 1", "[0] => Array", "[0] => app\index\controller\SObject Object", "[0] => app\index\controller\SObjectString Object","[0] => abcd", "[0] => Resource id", "[0] => Hello you! How are you today?"]), $log);
    }

    public function testException()
    {
        $this->get('/test_uncaught_exception');
        $log = $this->util->get_log();
        $this->assertTrue($this->util->check_error($log), $log);
        $this->assertTrue($this->util->check_span($log,["app\common\TestError::throwException", "EXP value:throw some thing"]), $log);
    }

    public function testCurl()
    {
        $_GET['remote'] = 'http%3A%2F%2Fwww.baidu.com';
        $this->get('/test_curl');
        $this->assertResponseOk();
        $log = $this->util->get_log();
        $this->assertTrue($this->util->check_error($log), $log);
        $this->assertTrue($this->util->check_span($log,["curl_exec", "key:dst", "nsid"]), $log);
    }

    public function testGuzzle()
    {
        $this->visit('/test_guzzle');
        $this->assertResponseOk();
        $log = $this->util->get_log();
        $this->assertTrue($this->util->check_error($log), $log);
        $this->assertTrue($this->util->check_span($log,["GuzzleHttp\Client::request","key:dst value:www.baidu.com", "nsid"]), $log);
    }

    public function testPDO()
    {
        $sql = "CREATE TABLE user( ".
        "id INT NOT NULL AUTO_INCREMENT, ".
        "name VARCHAR(100) NOT NULL, ".
        "age VARCHAR(40), ".
        "city VARCHAR(40), ".
        "PRIMARY KEY ( id ))ENGINE=InnoDB DEFAULT CHARSET=utf8; ";
        Db::connect('mysql://test:123456@127.0.0.1:3306/DBTest#utf8')->execute($sql);
        $this->visit('/test_pdo');
        $this->assertResponseOk();
        $log = $this->util->get_log();
        $this->assertTrue($this->util->check_error($log), $log);
        $this->assertTrue($this->util->check_span($log,["PDO::__construct","dst value:dev-mysql", "PDO::prepare", "PDOStatement::execute", "PDOStatement::fetchAll"]), $log);
    }

    public function testRedis()
    {
        $this->visit('/test_redis');
        $this->assertResponseOk();
        $log = $this->util->get_log();
        $this->assertTrue($this->util->check_error($log), $log);
        $this->assertTrue($this->util->check_span($log,["Redis::set","Redis::get"]), $log);
    }
}
