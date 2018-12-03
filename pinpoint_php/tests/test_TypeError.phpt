--TEST--
Test TypeError
--INI--
pinpoint_agent.pinpoint_enable=true
pinpoint_agent.trace_exception=true
display_errors =On
error_reporting = E_ALL
profiler.proxy.http.header.enable=true
pinpoint_agent.testCovered=1
--SKIPIF--
<?php
if (substr(phpversion(), 0, 1) != '7') die("skip this test is for php 7");
?>
--FILE--
<?php
class TestTypeErrorInterceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;
    public function __construct()
    {
        $this->apiId = pinpoint_add_api("testTypeError", 10); // functionName, lineno
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
                $event->addAnnotation(PINPOINT_ANNOTATION_RETURN,print_r($retArgs,true));
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
        $p = new TestTypeErrorInterceptor();
        $this->addInterceptor($p,"testTypeError",basename(__FILE__, '.php'));
    }
}

$p = new QuickStartPlugin();
pinpoint_add_plugin($p, basename(__FILE__, '.php'));
pinpint_aop_reload();

function testTypeError(int $integer)
{
    return $integer;
}
testTypeError("str");
?>
--EXPECTF--
request start
  addInterceptor name:[testTypeError] class:[test_TypeError]
  call testTypeError's interceptorPtr::onBefore
    setApiId:[%i]
    setServiceType:[1501]
    addAnnotation [-1]:[Array
(
    [0] =&gt; str
)
]
  call [TestTypeErrorInterceptor::onexception]
  setExceptionInfo:[Fatal error: Argument 1 passed to testTypeError() must be of the type integer, string given, called in %s]
  [EXCEPTION] file:[%s] line:[%d] msg:[Argument 1 passed to testTypeError() must be of the type integer, string given, called in %s]
  call testTypeError's interceptorPtr::onEnd
[ERROR] file:[%s] line:[%d] msg:[Undefined variable: retArgs]

Notice: Undefined variable: retArgs in %s on line %d
addAnnotation [14]:[]

Fatal error: Uncaught TypeError: Argument 1 passed to testTypeError() must be of the type integer, string given, called in%S
Stack trace:
#0 %s(%d): testTypeError('str')
#1 {main}
  thrown in %s on line %d
request shutdown