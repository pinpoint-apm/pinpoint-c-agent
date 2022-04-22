<?php
namespace app\index\controller;

require_once __DIR__.'/Args.php';

use app\common\AppDate;
use app\common\AopFunction;
use app\common\TestError;
use app\common\AccessRemote;
use app\common\Teacher;
use app\common\Doctor;
use app\common\Student;
use app\common\TestGenerator;
use app\common\AbstractStudent;
use app\common\InterfaceStudent;
use app\common\TestClone;
use app\common\TestRecursion;
use app\common\TestAnonymous;
use app\common\TestStatic;
use app\common\TestFinalClass;
use app\common\TestFinalFunc;
use app\common\TestTrait;
use app\common\CallBackFunc;
use app\common\LevelClass;
use app\common\ExtendLevel;
use app\common\OverRideLevel;
use app\common\TestArgs;
use app\common\TestReturn;
use app\common\TestGuzzle;
use app\index\model\User;

use think\Controller;
use think\facade\Cache;
use think\Db;

class Index extends Controller
{
    public function index()
    {
        return $this->fetch("index");
    }

    public function get_date()
    {
        return AppDate::outputDate();
    }

    public function test_func1()
    {
        $instance = new AopFunction();
        return $instance->test_func1("one", 3);
    }

    public function test_func2_caller()
    {
        $instance = new AopFunction();
        return $instance->test_func2();
    }

    public function test_inherit_func()
    {
        $instance = new Student();
        $r1 = $instance->eat();
        $instance = new Teacher();
        $r2 = $instance->eat();
        $instance = new Doctor();
        $r3 = $instance->eat();
        return $r1.$r2.$r3.$instance->other();

    }

    public function test_generator_func()
    {
        $instance = new TestGenerator();
        $sum = 0;
        foreach ($instance->generator(1, 10, 2) as $number){$sum += $number;}
        return $sum;
    }

    public function test_abstract_func()
    {
        $instance = new AbstractStudent("Evy");
        $r1 = $instance->eat();
        $r2 = $instance->drink();
        return $r1.$r2.$instance->breath();
    }

    public function test_interface_func()
    {
        $instance = new InterfaceStudent();
        $instance->setVariable("Evy", "Hua");
        $instance->setVariable("Sam", "Wei");
        $r1 = $instance->getHtml("Hello {Evy}");
        return $r1.$instance->other("Evy");
    }

    public function test_clone()
    {
        $instance1 = new TestClone();
        $instance2 = clone $instance1;
        $instance2->setColor("green");
        $r1 = $instance2->getColor();
        $instance1->setColor("red");
        return $r1.$instance1->getColor();
    }

    public function test_recursion()
    {
        $instance = new TestRecursion();
        $r1 = $instance->test_cumsum1(3);
//        return $instance->test_cumsum2(3);
        return $r1;
    }

    public function test_anonymous()
    {
        $instance = new TestAnonymous();
        $bell = $instance->createbell(date("Y/m/d"));
        return $bell();
    }

    public function test_static()
    {
        return TestStatic::static_func(123);
    }

    public function test_final_class()
    {
        $instance = new TestFinalClass();
        return $instance->test();
//        class swtest1 extends TestFinalClass{};
    }

    public function test_final_func()
    {
        $instance = new TestFinalFunc();
        return $instance->test("abc");
//        class swtest2 extends app\TestFinalFunc
//        {
//            public final function test(){
//                echo "123";
//            }
//        }
    }

    public function test_trait()
    {
        $instance = new TestTrait();
        $r1 = $instance->traitfunc("Evy");
        return $r1.$instance->test();
    }

    public function test_callback()
    {
        $className = "app\common\CallBackFunc";
        $funcName = "fnCallback1";
        $instance = new CallBackFunc();
        $r1 = call_user_func_array(array($className, $funcName), array("hello", "world"));
        $r2 = call_user_func_array(array($instance,"fnCallback2"), array("hello", "world"));
        return $r1.$r2;
    }

    public function test_level()
    {
        $instance = new LevelClass();
        $r1 = $instance->public_function(123);
        $instance = new ExtendLevel();
        $r2 = $instance->public_function(123);
        $instance = new OverRideLevel();
        return $instance->public_function(123);
    }

    public function test_args()
    {
        $myfile = fopen(__DIR__.'/Args.php', "r");
        $instance = new TestArgs();
        return $instance->test_args(null, 123, 3.1415, true, array(1, 2), new SObject(1), new SObjectString("ObjToString"), "abcd", $myfile, constant("GREETING"));
    }

    public function test_return()
    {
        $myfile = fopen(__DIR__.'/Args.php', "r");
        $instance = new TestReturn();
        $args = array(null, 123, 3.1415, true, array(1, 2), new SObject(1), new SObjectString("ObjToString"), "abcd", $myfile, constant("GREETING"));
        $return_array = [];
        foreach($args as $value){
            array_push($return_array,$instance->test_return($value));
        }
        return count($return_array);
    }

    public function test_curl()
    {
        $acr =  new AccessRemote();
        $acr->gotoUrl($_GET['remote']);
        return "Call ".$_GET['remote'];
    }

    public function test_guzzle()
    {
        $guzzle = new TestGuzzle();
        $tmp = $guzzle->gotoV();
//        return $guzzle->gotofoo();
        return $tmp."call remote";
    }

    public function test_pdo()
    {
        // $user =  new User;
        // $user->data(['name'=>'thinkphp','age'=>10, 'city'=>'thinkphp']);
        // $user->save();
        $user = new User;
        $user->name = 'thinkphp';
        $user->city    = 'test';
        $user->save();
        $user1 = User::get(['name'=>'thinkphp']);
        $user2 = User::get(function($query){
            $query->where('name', 'thinkphp');
        });
        return $user1;
    }

    public function test_redis()
    {
        Cache::set('name', 'value');
        return Cache::get('name');
    }

    public function test_mongo()
    {
        return User::get(1);
    }
    ###############################Test Exception###################
    public function test_call_undefined_function()
    {
        $error = new TestError();
        // try {
        $error->error();
        // $error->testParseError();
        // $error->testTypeError("abc");
        // $error->testArithmeticError();
        // $error->testDivisionByZeroError();
        // $error->testAssertionError();
        // }catch (Exception $ex){
        //     echo "Catch exception $ex";
        // }
    }

    public function test_uncaught_exception()
    {
        return TestError::throwException();
    }

    public function test_caught_exception()
    {
        TestError::caughtException();
    }

    public function test_exception_recursion()
    {
        TestError::test_cumsum_e1(3);
    }
}

