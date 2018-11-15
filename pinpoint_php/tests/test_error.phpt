--TEST--
Test pinpoint
--INI--
pinpoint_agent.pinpoint_enable=true
pinpoint_agent.trace_exception=true
display_errors =On
error_reporting = E_ERROR
profiler.proxy.http.header.enable=true
pinpoint_agent.testCovered=1

--SKIPIF--
<?php
if (substr(phpversion(), 0, 1) != '7') die("skip this test is for php 7");
?>
--FILE--
<?php

class GetDateInterceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;
    public function __construct()
    {
        $this->apiId = pinpoint_add_api("hello_phpt", 10); // functionName, lineno
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
            $event->addAnnotation(PINPOINT_ANNOTATION_ARGS, htmlspecialchars(print_r($args,true),ENT_QUOTES));

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
            if ($event)
            {
                if ($retArgs)
                {
                    $event->addAnnotation(PINPOINT_ANNOTATION_RETURN,sprintf("args:%s, return:%s ",print_r($args,true),print_r($retArgs,true)));
                }
                $event->markAfterTime();
                $trace->traceBlockEnd($event);
            }
        }
    }

    public function onException($callId, $exceptionStr)
    {

    }
}

class QuickStartPlugin extends \Pinpoint\Plugin
{
    public function __construct()
    {
        parent::__construct();
        $p = new GetDateInterceptor();
        $this->addInterceptor($p,"hello_phpt",basename(__FILE__, '.php'));
    }
}

$p = new QuickStartPlugin();
pinpoint_add_plugin($p, basename(__FILE__, '.php'));
pinpint_aop_reload();


function hello_phpt($who)
{
    echo "hello " .$who."\n";
    return $who;
}
hello_phpt();


--EXPECTF--
request start
  addInterceptor name:[hello_phpt] class:[test_error]
  call hello_phpt's interceptorPtr::onBefore
    setApiId:[-2]
    setServiceType:[1501]
    addAnnotation [-1]:[Array
(
)
]
  call [GetDateInterceptor::onexception]
  [EXCEPTION] file:[%s] line:[%d] msg:[Too few arguments to function hello_phpt(), 0 passed in %s on line 70 and exactly 1 expected]
  call hello_phpt's interceptorPtr::onEnd

Fatal error: Uncaught ArgumentCountError: Too few arguments to function hello_phpt(), 0 passed in %s on line 70 and exactly 1 expected in %s:%d
Stack trace:
#0 %s(70): hello_phpt()
#1 {main}
  thrown in %s on line %d
request shutdown