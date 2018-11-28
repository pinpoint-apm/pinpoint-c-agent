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
request start
  addInterceptor name:[test_cumsum] class:[test_recursive]
  call test_cumsum's interceptorPtr::onBefore
    setApiId:[%i]
    setServiceType:[1501]
    addAnnotation [-1]:[Array
(
    [0] =&gt; 3
)
]
  call test_cumsum's interceptorPtr::onBefore
    setApiId:[%i]
    setServiceType:[1501]
    addAnnotation [-1]:[Array
(
    [0] =&gt; 2
)
]
  call test_cumsum's interceptorPtr::onBefore
    setApiId:[%i]
    setServiceType:[1501]
    addAnnotation [-1]:[Array
(
    [0] =&gt; 1
)
]
  call test_cumsum's interceptorPtr::onEnd
    addAnnotation [14]:[1]
  call test_cumsum's interceptorPtr::onEnd
    addAnnotation [14]:[3]
  call test_cumsum's interceptorPtr::onEnd
    addAnnotation [14]:[6]
6
request shutdown