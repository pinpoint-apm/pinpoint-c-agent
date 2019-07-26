<?php
/**
 * User: eeliu
 * Date: 1/4/19
 * Time: 2:29 PM
 */

define('AUTOLOAD_FILE_ALIAS',__DIR__."/../vendor/autoload.php");
define('AOP_CACHE_DIR',__DIR__.'/Cache/');
define('PLUGINS_DIR',__DIR__.'/../Plugins/');

require_once __DIR__. '/../vendor/eeliu/php_simple_aop/auto_pinpointed.php';

//require_once AUTOLOAD_FILE_ALIAS;
use app\AppDate;
use app\AopFunction;
use app\TestError;
use app\AccessRemote;


$type = null;
if(isset($_GET["type"]))
{
    $type =  $_GET["type"];
}

//if (!function_exists('getallheaders'))
//{
//    function getallheaders()
//    {
//        $headers = [];
//        foreach ($_SERVER as $name => $value)
//        {
//            $headers[str_replace(' ', '-', ucwords(strtolower(str_replace('_', ' ', substr($name, 5)))))] = $value;
//        }
//        return $headers;
//    }
//}

//print_r(getallheaders());


switch($type)
{
    case 'get_date':
        AppDate::outputDate();
        break;
    case 'test_func1':
        $instance  = new AopFunction();
        $instance->test_func1("one",2);
        break;
    case 'test_func2_caller':
        $instance  = new AopFunction();
        $instance->test_func2();
        $instance->getReturnDescription();
        break;
    case 'test_exception':
        TestError::test_cumsum_e1(123);
        break;
    case 'test_call_undefined_function':
        $error = new TestError();

        try {
            $error->testExpInDeepLevel();
            $p = 123;
            $error->testRetConst($p, '23');
            $error->testRetParm($p, '23');
            $error->testRetConstAndExp($p);
        }catch (Exception $ex){
            echo "Catch exception $ex";
        }

        break;
    case 'test_call_static':
        $args = ["123",3345];
        TestError::Strict($args);
        break;
    case 'test_curl':
        $acr =  new AccessRemote();
        $acr->gotoUrl($_GET['remote']);
        break;
    case "test_db":
        $db = new \app\DBcontrol("dev-test-mysql","root","root");
        $db->connectDb();
        $db->getData1();
        $db->getData2();
        break;
    default:
        echo "not supported type $type";
}