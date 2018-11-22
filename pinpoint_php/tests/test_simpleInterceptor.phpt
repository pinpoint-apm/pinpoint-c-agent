--TEST--
Test pinpoint
--INI--
pinpoint_agent.pinpoint_enable=true
pinpoint_agent.trace_exception=true
profiler.proxy.http.header.enable=true
pinpoint_agent.testCovered=1
--FILE--
<?php

class QuickStartPlugin extends \Pinpoint\Plugin
{
    public function __construct()
    {
        parent::__construct();

        $this->addSimpleInterceptor("test_simple_interceptor", -1);
    }
}

$p = new QuickStartPlugin();
pinpoint_add_plugin($p, basename(__FILE__, '.php'));
pinpint_aop_reload();

define("GREETING","Hello you! How are you today?");

class SObject
{
    var $x_;

    public function __construct($x)
    {
        $this->x_ = $x;
    }
};

class SObjectString
{
    var $x_;

    public function __construct($x)
    {
        $this->x_ = $x;
    }

    public function __toString()
    {
        return $this->x_;
    }

};

function test_simple_interceptor($null_arg, $long_arg, $double_arg, $bool_arg,
                                 $array_arg, $object_arg, $object_tostring_arg, $string_arg, $resource_arg, $constant_arg)
{
    return "return xxx";
}
$myfile = fopen(__FILE__, "r");
test_simple_interceptor(null, 123, 3.1415, true, array(1, 2), new SObject(1), new SObjectString(1.67), "abcd",
    $myfile, constant("GREETING"));
fclose($myfile);
?>

--EXPECTF--
%Srequest start
%SaddSimpleInterceptor name:[test_simple_interceptor]
%Scall test_simple_interceptor's interceptorPtr::onBefore
    SimpleInterceptor->addAnnotation key:[-1] value:[{[0]:NULL [1]:123 [2]:3.1415 [3]:true [4]:array(...) [5]:object%SSObject%S [6]:object%SSObjectString%S [7]:abcd [8]:resource(stream) [9]:Hello you! How are you today? }] 
%Scall test_simple_interceptor's interceptorPtr::onEnd
%SSimpleInterceptor->addAnnotation key:[14] value:[return xxx] 

request shutdown