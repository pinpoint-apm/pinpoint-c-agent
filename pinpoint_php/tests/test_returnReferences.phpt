--TEST--
Test Returns Reference
--INI--
pinpoint_agent.pinpoint_enable=true
pinpoint_agent.trace_exception=true
profiler.proxy.http.header.enable=true
pinpoint_agent.testCovered=1
--FILE--
<?php

class TestGeneratorInterceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;
    var $save_event;
    public function __construct()
    {
        $this->apiId = pinpoint_add_api("MyClass::returns_reference", 2); // functionName, lineno
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
            if ($event)
            {
                $event->addAnnotation(PINPOINT_ANNOTATION_RETURN, htmlspecialchars(print_r($retArgs,true),ENT_QUOTES));
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
//                $trace->traceBlockEnd($event);
            }
        }
    }
}


class QuickStartPlugin extends \Pinpoint\Plugin
{
    public function __construct()
    {
        parent::__construct();
        $p = new TestGeneratorInterceptor();
        $this->addInterceptor($p,"MyClass::returns_reference",basename(__FILE__, '.php'));
    }
}

$p = new QuickStartPlugin();
pinpoint_add_plugin($p, basename(__FILE__, '.php'));
pinpint_aop_reload();

class MyClass{
    public $someref = 1;
    function &returns_reference()
    {
        return $this->someref;
    }
}
$a = new MyClass();
$newref = &$a->returns_reference();
?>
--EXPECTF--
request start
  addInterceptor name:[MyClass::returns_reference] class:[test_returnReferences]
  call MyClass::returns_reference's interceptorPtr::onBefore
    setApiId:[-2]
    setServiceType:[1501]
    addAnnotation [-1]:[Array
(
)
]
  call MyClass::returns_reference's interceptorPtr::onEnd
    addAnnotation [14]:[1]
request shutdown