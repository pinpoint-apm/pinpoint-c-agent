--TEST--
Test pinpoint
--INI--
pinpoint_agent.pinpoint_enable=true
pinpoint_agent.trace_exception=true
profiler.proxy.http.header.enable=true
pinpoint_agent.testCovered=1

--FILE--
<?php

class TestCumsumInterceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;
    public function __construct()
    {
        $this->apiId = pinpoint_add_api("test_cumsum", -1); // functionName, lineno
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
                    $event->addAnnotation(PINPOINT_ANNOTATION_RETURN, htmlspecialchars(print_r($retArgs,true),ENT_QUOTES));
                }
                $event->markAfterTime();
                $trace->traceBlockEnd($event);
            }
        }
    }

    public function onException($callId, $exceptionStr)
    {
        pinpoint_log(PINPOINT_INFO, "call TestExceptionInterceptor onException.");
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
        $p = new TestCumsumInterceptor();
        $this->addInterceptor($p,"test_cumsum",basename(__FILE__, '.php'));
    }
}

$p = new QuickStartPlugin();
pinpoint_add_plugin($p, basename(__FILE__, '.php'));
pinpint_aop_reload();

function test_cumsum($n)
{
    if ($n == 1) return 1;
    return $n + test_cumsum($n - 1);
}
echo test_cumsum(3);
?>

--EXPECTF--
%Srequest start
%SaddInterceptor name:[test_cumsum] class:[test_recursive]
%Scall test_cumsum's interceptorPtr::onBefore
%SsetApiId:[%s]
%SsetServiceType:[1501]
%SaddAnnotation [-1]:[Array
%S(
%S[0] =&gt; 3
%S)
%S]
%Scall test_cumsum's interceptorPtr::onBefore
%SsetApiId:[%s]
%SsetServiceType:[1501]
%SaddAnnotation [-1]:[Array
%S(
%S[0] =&gt; 2
%S)
%S]
%Scall test_cumsum's interceptorPtr::onBefore
%SsetApiId:[%s]
%SsetServiceType:[1501]
%SaddAnnotation [-1]:[Array
%S(
%S[0] =&gt; 1
%S)
%S]
%Scall test_cumsum's interceptorPtr::onEnd
%SaddAnnotation [14]:[1]
%Scall test_cumsum's interceptorPtr::onEnd
%SaddAnnotation [14]:[3]
%Scall test_cumsum's interceptorPtr::onEnd
%SaddAnnotation [14]:[6]
%S6
%Srequest shutdown