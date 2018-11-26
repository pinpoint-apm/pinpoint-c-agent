--TEST--
Test pinpoint
--INI--
pinpoint_agent.pinpoint_enable=true
pinpoint_agent.trace_exception=true
profiler.proxy.http.header.enable=true
pinpoint_agent.testCovered=1

--FILE--
<?php

class TestFinal1Interceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;
    var $save_event;
//    public function TestFunc1Interceptor()
    function __construct()
    {
        $this->apiId = pinpoint_add_api("BaseClass::finalFunction", 32); // functionName, lineno
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

            $event->addAnnotation(PINPOINT_ANNOTATION_ARGS, print_r($args,true));


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

class TestFinal2Interceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;
    var $save_event;
//    public function TestFunc1Interceptor()
    function __construct()
    {
        $this->apiId = pinpoint_add_api("finalClass::func", 32); // functionName, lineno
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

            $event->addAnnotation(PINPOINT_ANNOTATION_ARGS, print_r($args,true));


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
            }
        }
    }
}

class QuickStartPlugin extends Pinpoint\Plugin
{
    public function __construct()
    {
        parent::__construct();

        $i = new TestFinal1Interceptor();
        $this->addInterceptor($i, "BaseClass::finalFunction", basename(__FILE__, '.php'));

        $i = new TestFinal2Interceptor();
        $this->addInterceptor($i, "finalClass::func", basename(__FILE__, '.php'));
    }
}

$p = new QuickStartPlugin();
pinpoint_add_plugin($p, basename(__FILE__, '.php'));
pinpint_aop_reload();

class BaseClass {
    final public function finalFunction($value) {
        return $value;
    }
}

final class finalClass{
    public function func($value){
        return $value;
    }
}

$a = new BaseClass();
$a->finalFunction(10);

$b = new finalClass();
$b->func("abc");

?>
--EXPECTF--
request start
%SaddInterceptor name:[BaseClass::finalFunction] class:[test_final]
%SaddInterceptor name:[finalClass::func] class:[test_final]
%Scall BaseClass::finalFunction's interceptorPtr::onBefore
%SsetApiId:[%i]
%SsetServiceType:[1501]
%SaddAnnotation [-1]:[Array
(
%S[0] => 10
)
]
%Scall BaseClass::finalFunction's interceptorPtr::onEnd
%SaddAnnotation [14]:[10]
%Scall finalClass::func's interceptorPtr::onBefore
%SsetApiId:[%i]
%SsetServiceType:[1501]
%SaddAnnotation [-1]:[Array
(
%S[0] => abc
)
]
%Scall finalClass::func's interceptorPtr::onEnd
%SaddAnnotation [14]:[abc]
request shutdown