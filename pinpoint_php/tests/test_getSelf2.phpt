--TEST--
Test pinpoint
--INI--
pinpoint_agent.pinpoint_enable=true
pinpoint_agent.trace_exception=true
profiler.proxy.http.header.enable=true
pinpoint_agent.testCovered=1

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
            $getSelf = $this->getSelf();
            $event = $trace->traceBlockBegin($callId);
            $event->markBeforeTime();
            $event->setApiId($this->apiId);
            $event->setServiceType(PINPOINT_PHP_RPC_TYPE);
            $event->addAnnotation(PINPOINT_ANNOTATION_ARGS, print_r($getSelf,true));
            $this->save_event = $event;
        }
    }

    public function onEnd($callId, $data)
    {

        $trace = pinpoint_get_current_trace();
        if ($trace)
        {
            $getSelf = $this->getSelf();
            $args = $data["args"];
            $retArgs = $data["result"];
            $event = $trace->getEvent($callId);
            $event->addAnnotation(PINPOINT_ANNOTATION_RETURN, htmlspecialchars(print_r($getSelf,true),ENT_QUOTES));
            $event->markAfterTime();
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
                $getSelf = $this->getSelf();
                $event->markAfterTime();
                $event->setExceptionInfo($exceptionStr);
                $event->addAnnotation(PINPOINT_ANNOTATION_RETURN, htmlspecialchars(print_r($getSelf,true),ENT_QUOTES));
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
        try{
            throw new Exception("I am Exception!");
        }catch (Exception $e){
            echo $e->getMessage();
        }
        return $value.$this->name;
    }
}

$a = new MyClass();
$a->testFunc("hello");

?>
--EXPECTF--
request start
%SaddInterceptor name:[MyClass::testFunc] class:[test_getSelf2]
%Scall MyClass::testFunc's interceptorPtr::onBefore
%SsetApiId:[%i]
%SsetServiceType:[1501]
    addAnnotation [-1]:[MyClass Object
(
    [name] => Evy
)
]
  call [TestGetSelfInterceptor::onexception]
%s[ERROR] call getself not in before/end method.
  setExceptionInfo:[Fatal error: I am Exception! in %s]
  addAnnotation [14]:[]
  [EXCEPTION] file:[%s] line:[%d] msg:[I am Exception!]
I am Exception!  call MyClass::testFunc's interceptorPtr::onEnd
    addAnnotation [14]:[MyClass Object
(
    [name] =&gt; Evy
)
]
request shutdown