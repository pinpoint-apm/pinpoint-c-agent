--TEST--
Test pinpoint
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
class TestParseErrorInterceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;
    public function __construct()
    {
        $this->apiId = pinpoint_add_api("testParseError", 10); // functionName, lineno
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
        $p = new TestParseErrorInterceptor();
        $this->addInterceptor($p,"testParseError",basename(__FILE__, '.php'));
    }
}

$p = new QuickStartPlugin();
pinpoint_add_plugin($p, basename(__FILE__, '.php'));
pinpint_aop_reload();

function testParseError()
{
    $var = "test";
    $str = 'This is a $var!';
    eval("$str = \"$str\";");
    echo $str."\n";
}
testParseError();
?>
--EXPECTF--
%Srequest start
%SaddInterceptor name:[testParseError] class:[test_ParseError]
%Scall testParseError's interceptorPtr::onBefore
%SsetApiId:[%s]
%SsetServiceType:[1501]
%SaddAnnotation [-1]:[Array
%S(
%S)
%S]
%Scall [TestParseErrorInterceptor::onexception]
%SsetExceptionInfo:[Fatal error: syntax error, unexpected 'is' (T_STRING) in %s]
%S[EXCEPTION] file:[%s] line:[%d] msg:[syntax error, unexpected 'is' (T_STRING)]
%Scall testParseError's interceptorPtr::onEnd
%A
%SaddAnnotation [14]:[]
%A
Parse error: syntax error, unexpected 'is' (T_STRING) in %s
request shutdown