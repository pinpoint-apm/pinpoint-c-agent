--TEST--
Test pinpoint
--INI--
pinpoint_agent.pinpoint_enable=true
pinpoint_agent.trace_exception=true
profiler.proxy.http.header.enable=true
pinpoint_agent.testCovered=1
--SKIPIF--
<?php
if (substr(phpversion(), 0, 1) != '7') die("skip this test is for php 7");
?>
--FILE--
<?php

class TestGetSelfInterceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;
    var $save_event;
    function __construct()
    {
        $this->apiId = pinpoint_add_api("MyClass::testFunc", 32); // functionName, lineno
    }

    public function onBefore($callId, $args)
    {
        $trace = pinpoint_get_current_trace();
        if ($trace)
        {
            $event = $trace->traceBlockBegin($callId);
            $event->markBeforeTime();
            $event->setApiId($this->apiId);
            $event->setServiceType(PINPOINT_PHP_RPC_TYPE);
            $event->addAnnotation(PINPOINT_ANNOTATION_ARGS, print_r($args,true));
            $this->save_event = $event;
        }
    }

    public function onEnd($callId, $data)
    {

        $trace = pinpoint_get_current_trace();
        if ($trace)
        {
            $args = $data["args"];
            $retArgs = $data["result"];
            $event = $trace->getEvent($callId);
            $event->addAnnotation(PINPOINT_ANNOTATION_RETURN, sprintf("args:%s",print_r($args,true)));
            $event->markAfterTime();
            xxx();
            $trace->traceBlockEnd($event);
        }
    }

    public function onException($callId, $exceptionStr)
    {
        $trace = pinpoint_get_current_trace();
        if ($trace)
        {
            $event = $trace->getEvent($callId);
            if ($event)
            {
                $event->markAfterTime();
                $event->setExceptionInfo($exceptionStr);
//                $trace->traceBlockEnd($event);
            }
        }
    }
}

class QuickStartPlugin extends Pinpoint\Plugin
{
    public function __construct()
    {
        parent::__construct();

        $i = new TestGetSelfInterceptor();
        $this->addInterceptor($i, "MyClass::testFunc", basename(__FILE__, '.php'));
    }
}

$p = new QuickStartPlugin();
pinpoint_add_plugin($p, basename(__FILE__, '.php'));
pinpint_aop_reload();

class MyClass {
    public $name = "Evy";
    public function testFunc($value) {
        throw new Exception("I am Exception!");
        return $value.$this->name;
    }
}
$a = new MyClass();
$a->testFunc("hello");

?>
--EXPECTF--
request start
  addInterceptor name:[MyClass::testFunc] class:[test_error_onEnd]
  call MyClass::testFunc's interceptorPtr::onBefore
    setApiId:[-2]
    setServiceType:[1501]
    addAnnotation [-1]:[Array
(
    [0] => hello
)
]
  call [TestGetSelfInterceptor::onexception]
  setExceptionInfo:[Fatal error: I am Exception! in %s on line %d]
  [EXCEPTION] file:[%s] line:[%d] msg:[I am Exception!]
  call MyClass::testFunc's interceptorPtr::onEnd
    addAnnotation [14]:[args:Array
(
    [0] => hello
)
]
    call [TestGetSelfInterceptor::onexception]
    setExceptionInfo:[Fatal error: Call to undefined function xxx() in %s on line %d]
    [EXCEPTION] file:[%s] line:[%d] msg:[Call to undefined function xxx()]
%s %s [pinpoint] [%s] php_common.cpp:%d [ERROR] w_zend_call_method throw: [exception: Call to undefined function xxx() in %s on line %d]
%s %s [pinpoint] [%s] php_interfaces.cpp:%d [ERROR] Interceptor name=[MyClass::testFunc] onend failed!!! please check your code!!!
%s %s [pinpoint] [%s] trace.cpp:%d [ERROR] mark bad trace !!! check your plugins...

Fatal error: Uncaught Exception: I am Exception! in %s:%d
Stack trace:
#0 %s(%d): MyClass->testFunc('hello')
#1 {main}

Next Error: Call to undefined function xxx() in %s:%d
Stack trace:
#0 %s(%d): TestGetSelfInterceptor->onEnd(%d, Array)
#1 {main}
  thrown in %s on line %d
request shutdown