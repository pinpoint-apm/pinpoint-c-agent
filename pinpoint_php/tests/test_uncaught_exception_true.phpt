--TEST--
Test Uncaught Exception
--INI--
pinpoint_agent.pinpoint_enable=true
pinpoint_agent.trace_exception=true
profiler.proxy.http.header.enable=true
pinpoint_agent.testCovered=1

--FILE--
<?php
class TestExceptionInterceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;
    public function __construct()
    {
        $this->apiId = pinpoint_add_api("testException", 10); // functionName, lineno
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
                $event->addAnnotation(PINPOINT_ANNOTATION_RETURN,sprintf("args:%s",print_r($args,true)));
                $event->markAfterTime();
                $trace->traceBlockEnd($event);
            }
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
            }
        }
    }
}

class QuickStartPlugin extends \Pinpoint\Plugin
{
    public function __construct()
    {
        parent::__construct();

        $p = new TestExceptionInterceptor();
        $this->addInterceptor($p,"testException",basename(__FILE__, '.php'));
    }
}

$p = new QuickStartPlugin();
pinpoint_add_plugin($p, basename(__FILE__, '.php'));
pinpint_aop_reload();

function testException($num)
{
    $num = "hello world!";
    throw new Exception("This is an uncaught exception!");
    return $num;
}

testException("Mike");
--EXPECTF--
request start
  addInterceptor name:[testException] class:[test_uncaught_exception_true]
  call testException's interceptorPtr::onBefore
    setApiId:[%i]
    setServiceType:[1501]
    addAnnotation [-1]:[Array
(
    [0] =&gt; Mike
)
]
  call [TestExceptionInterceptor::onexception]
  setExceptionInfo:[Fatal error: This is an uncaught exception! in %s]
  [EXCEPTION] file:[%s] line:[%d] msg:[This is an uncaught exception!]
  call testException's interceptorPtr::onEnd
    addAnnotation [14]:[args:Array
(
    [0] => %s
)
]

Fatal error: Uncaught%SException%SThis is an uncaught exception!%s
Stack trace:
#0 %s(%d): testException('hello world!')
#1 {main}
  thrown in %s on line %d
request shutdown