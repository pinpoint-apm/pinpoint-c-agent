--TEST--
Test pinpoint
--INI--
pinpoint_agent.pinpoint_enable=true
pinpoint_agent.trace_exception=true
profiler.proxy.http.header.enable=true
pinpoint_agent.testCovered=1

--FILE--
<?php
class TestFunc1Interceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;
    public function __construct()
    {
        $this->apiId = pinpoint_add_api("testFunc", 10); // functionName, lineno
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
            $self = $this->getSelf();
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
                $event->addAnnotation(PINPOINT_ANNOTATION_RETURN, htmlspecialchars(print_r($args,true),ENT_QUOTES));
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
        $i = new TestFunc1Interceptor();
        $this->addInterceptor($i, "testFunc", basename(__FILE__, '.php'));
    }
}

$p = new QuickStartPlugin();
pinpoint_add_plugin($p, basename(__FILE__, '.php'));
pinpint_aop_reload();


function testFunc($arg1, $arg2)
{
    unset($arg1, $arg2);
    echo "Arguments has been unset!";
}

$f = fopen(__FILE__, "r");
testFunc("one", $f);
fclose($f);
?>
--EXPECTF--
%Srequest start
%SaddInterceptor name:[testFunc] class:[test_args_onEnd1]
%Scall testFunc's interceptorPtr::onBefore
%SsetApiId:[-2]
%SsetServiceType:[1501]
%SaddAnnotation [-1]:[Array
(
%S[0] =&gt; one
%S[1] =&gt; Resource id #5
)
]
Arguments has been unset!  call testFunc's interceptorPtr::onEnd
%SaddAnnotation [14]:[Array
(
%S[0] =&gt; one
%S[1] =&gt; Resource id #5
)
]
request shutdown