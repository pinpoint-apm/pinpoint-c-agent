<?php
/**
 * User: eeliu
 * Date: 1/4/19
 * Time: 2:29 PM
 */

define('AUTOLOAD_FILE_ALIAS',__DIR__."/../vendor/autoload.php");
define('AOP_CACHE_DIR',__DIR__.'/../Cache/');
define('PLUGINS_DIR',__DIR__.'/../Plugins/');

require_once __DIR__. '/../vendor/eeliu/php_simple_aop/auto_pinpointed.php';
require_once __DIR__.'/Args.php';
//require_once __DIR__."/../vendor/autoload.php";

//require_once AUTOLOAD_FILE_ALIAS;
use app\AppDate;
use app\AopFunction;
use app\TestError;
use app\AccessRemote;
use app\Teacher;
use app\Doctor;
use app\Student;
use app\TestGenerator;
use app\AbstractStudent;
use app\InterfaceStudent;
use app\TestClone;
use app\TestRecursion;
use app\TestAnonymous;
use app\TestStatic;
use app\TestFinalClass;
use app\TestFinalFunc;
use app\TestTrait;
use app\CallBackFunc;
use app\LevelClass;
use app\ExtendLevel;
use app\OverRideLevel;
use app\TestArgs;
use app\TestReturn;
use app\TestPDO;
use app\TestMysqli;
use app\TestRedis;

$type = null;
if(isset($_GET["type"]))
{
    $type =  $_GET["type"];
}

if (!function_exists('getallheaders'))
{
    function getallheaders()
    {
        $headers = [];
        foreach ($_SERVER as $name => $value)
        {
            $headers[str_replace(' ', '-', ucwords(strtolower(str_replace('_', ' ', substr($name, 5)))))] = $value;
        }
        return $headers;
    }
}

//print_r(getallheaders());


//if(isset($_SERVER[PINPOINTID]) && isset($_SERVER[NEXTSPAN]))
//{
//    echo $_SERVER[PINPOINTID] , $_SERVER[NEXTSPAN];
//}

switch($type)
{
    case 'get_date':
        AppDate::outputDate();
        break;
    case 'test_func1':
        $instance = new AopFunction();
        echo $instance->test_func1("one", 3);
        break;
    case 'test_func2_caller':
        $instance = new AopFunction();
        $instance->test_func2();
        break;
    case 'test_inherit_func':
        $instance = new Student();
        $instance->eat();
        $instance = new Teacher();
        $instance->eat();
        $instance = new Doctor();
        $instance->eat();
        $instance->other();
        break;
    case 'test_generator_func':
        $instance = new TestGenerator();
        foreach ($instance->generator(1, 10, 2) as $number){echo $number;}
        break;
    case 'test_abstract_func':
        $instance = new AbstractStudent("Evy");
        echo $instance->eat();
        echo $instance->drink();
        echo $instance->breath();
        break;
    case 'test_interface_func':
        $instance = new InterfaceStudent();
        $instance->setVariable("Evy", "Hua");
        $instance->setVariable("Sam", "Wei");
        echo $instance->getHtml("Hello {Evy}");
        echo $instance->other("Evy");
        break;
    case 'test_clone':
        $instance1 = new TestClone();
        $instance2 = clone $instance1;
        $instance2->setColor("green");
        echo $instance2->getColor();
        $instance1->setColor("red");
        echo $instance1->getColor();
        break;
    case 'test_recursion':
        $instance = new TestRecursion();
        echo $instance->test_cumsum1(3);
        echo $instance->test_cumsum2(3);
        break;
    case 'test_anonymous':
        $instance = new TestAnonymous();
        $bell = $instance->createbell(date("Y/m/d"));
        $bell();
        break;
    case 'test_static':
        TestStatic::static_func(123);
        break;
    case 'test_final_class':
        $instance = new TestFinalClass();
        $instance->test();
//        class swtest1 extends TestFinalClass{};
        break;
    case 'test_final_func':
        $instance = new TestFinalFunc();
        $instance->test();
//        class swtest2 extends app\TestFinalFunc
//        {
//            public final function test(){
//                echo "123";
//            }
//        }
        break;
    case 'test_trait':
        $instance = new TestTrait();
        echo $instance->traitfunc("Evy");
        $instance->test();
        break;
    case 'test_callback':
        $className = "app\CallBackFunc";
        $funcName = "fnCallback1";
        $instance = new CallBackFunc();
        call_user_func_array(array($className, $funcName), array("hello", "world"));
        call_user_func_array(array($instance,"fnCallback2"), array("hello", "world"));
        break;
    case 'test_level':
        $instance = new LevelClass();
        $instance->public_function(123);
        $instance = new ExtendLevel();
        $instance->public_function(123);
        $instance = new OverRideLevel();
        $instance->public_function(123);
        break;
    case 'test_args':
        $myfile = fopen(__DIR__.'/Args.php', "r");
        $instance = new TestArgs();
        $instance->test_atgs(null, 123, 3.1415, true, array(1, 2), new SObject(1), new SObjectString("ObjToString"), "abcd", $myfile, constant("GREETING"));
        break;
    case 'test_return':
        $myfile = fopen(__DIR__.'/Args.php', "r");
        $instance = new TestReturn();
        $args = array(null, 123, 3.1415, true, array(1, 2), new SObject(1), new SObjectString("ObjToString"), "abcd", $myfile, constant("GREETING"));
        foreach($args as $value){
            var_dump($instance->test_return($value));
        }
        break;
    case 'test_uncaught_exception':
        TestError::throwException();
        break;
    case 'test_caught_exception':
        TestError::caughtException();
        break;
    case 'test_exception_recursion':
        TestError::test_cumsum_e1(3);
        break;
    case 'test_call_undefined_function':
        $error = new TestError();
        try {
            $error->error();
            $error->test_cumsum_e1(3);
            $p = 123;
            $error->testRetConst($p, '23');
            $error->testRetParm($p, '23');
            $error->testRetConstAndExp($p);
            $error->testRetArgAndExp($p);
            $error->testParseError();
            $error->testTypeError("abc");
            $error->testArithmeticError();
            $error->testDivisionByZeroError();
            $error->testAssertionError();
        }catch (Exception $ex){
            echo "Catch exception $ex";
        }
        break;
    case 'test_call_static':
        TestError::Strict();
        break;
    case 'test_curl':
        $acr =  new AccessRemote();
        $acr->gotoUrl($_GET['remote']);
        break;
    case 'test_pdo':
        $pdo =  new TestPDO();
        $pdo->connectDb();
        var_dump($pdo->getData1());
        echo "<br>";
        var_dump($pdo->getData2("select name from user;"));
        echo "<br>";
        var_dump($pdo->getData3("select * from user;"));
        $pdo->test();
        break;
    case 'test_mysqli':
        $pdo =  new TestMysqli();
        $pdo->connectDb();
        var_dump($pdo->getData1());
        echo "<br>";
        var_dump($pdo->getData2("select * from user;"));
        break;
    case 'test_redis':
        $rd = new TestRedis();
        $rd->connect_redis();
        $rd->setdata("cat", "HuaMer");
        $rd->getdata("cat");
        $rd->getkey();
        $rd->deldata("cat");
        break;
    default:
        echo "not supported type $type";
}

