--TEST--
Test AssertionError
--INI--
pinpoint_agent.pinpoint_enable=true
pinpoint_agent.trace_exception=true
display_errors =On
error_reporting = E_ALL
profiler.proxy.http.header.enable=true
pinpoint_agent.testCovered=1
zend.assertions=1
assert.exception=1
--SKIPIF--
<?php
if (substr(phpversion(), 0, 1) != '7') die("skip this test is for php 7");
?>
--FILE--
<?php
class TestAssertionErrorInterceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;
    public function __construct()
    {
        $this->apiId = pinpoint_add_api("testAssertionError", 10); // functionName, lineno
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
        $p = new TestAssertionErrorInterceptor();
        $this->addInterceptor($p,"testAssertionError",basename(__FILE__, '.php'));
    }
}

$p = new QuickStartPlugin();
pinpoint_add_plugin($p, basename(__FILE__, '.php'));
pinpint_aop_reload();

function testAssertionError()
{
    $value = 1;
    assert($value === 0);
}
testAssertionError();
?>
--EXPECTF--
request start
  addInterceptor name:[testAssertionError] class:[test_AssertionError]
  call testAssertionError's interceptorPtr::onBefore
    setApiId:[%i]
    setServiceType:[1501]
    addAnnotation [-1]:[Array
(
)
]
  call [TestAssertionErrorInterceptor::onexception]
  setExceptionInfo:[Fatal error: assert($value === 0) in %s on line %d]
  [EXCEPTION] file:[%s] line:[%d] msg:[assert($value === 0)]
  call testAssertionError's interceptorPtr::onEnd
[ERROR] file:[%s] line:[%d] msg:[Undefined variable: retArgs]

Notice: Undefined variable: retArgs in %s on line %d
addAnnotation [14]:[]

Fatal error: Uncaught AssertionError: assert($value === 0) in %s
Stack trace:
#0 %s(%d): assert(false, 'assert($value =...')
#1 %s(%d): testAssertionError()
#2 {main}
  thrown in %s on line %d
request shutdown