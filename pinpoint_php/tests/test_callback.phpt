--TEST--
Test pinpoint
--INI--
pinpoint_agent.pinpoint_enable=true
pinpoint_agent.trace_exception=true
profiler.proxy.http.header.enable=true
pinpoint_agent.testCovered=1
--FILE--
<?php

class TestCallback1Interceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;
    public function __construct()
    {
        $this->apiId = pinpoint_add_api("fnCallback", 10); // functionName, lineno
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
                    $event->addAnnotation(PINPOINT_ANNOTATION_RETURN,sprintf("args:%s, return:%s ",print_r($args,true),print_r($retArgs,true)));
                }
                $event->markAfterTime();
                $trace->traceBlockEnd($event);
            }
        }
    }

    public function onException($callId, $exceptionStr)
    {

    }
}

class TestCallbackInterceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;
    var $save_event;
    public function __construct()
    {
        $this->apiId = pinpoint_add_api("MyClass::fnCallBack", 2); // functionName, lineno
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
            }
        }
    }
}

class QuickStartPlugin extends \Pinpoint\Plugin
{
    public function __construct()
    {
        parent::__construct();
        $p = new TestCallback1Interceptor();
        $this->addInterceptor($p,"fnCallback",basename(__FILE__, '.php'));
        $p = new TestCallbackInterceptor();
        $this->addInterceptor($p,"MyClass::fnCallBack",basename(__FILE__, '.php'));
    }
}

$p = new QuickStartPlugin();
pinpoint_add_plugin($p, basename(__FILE__, '.php'));
pinpint_aop_reload();

function fnCallback($msg1, $msg2){
    return $msg1.$msg2;
}
call_user_func_array("fnCallback", array("hello","world"));

class MyClass{
    function fnCallBack($msg1, $msg2){
        return $msg1.$msg2;
    }
}
$className = new MyClass();
call_user_func_array(array($className, "fnCallBack"), array("hello", "world"));

?>

--EXPECTF--
%Srequest start
%SaddInterceptor name:[fnCallback] class:[test_callback]
%SaddInterceptor name:[MyClass::fnCallBack] class:[test_callback]
%Scall fnCallback's interceptorPtr::onBefore
%SsetApiId:[%s]
%SsetServiceType:[1501]
%SaddAnnotation [-1]:[Array
%S(
%S[0] =&gt; hello
%S[1] =&gt; world
%S)
%S]
%Acall fnCallback's interceptorPtr::onEnd
%SaddAnnotation [14]:[args:Array
(
%S[0] => hello
%S[1] => world
)
%S, return:helloworld ]
%Scall MyClass::fnCallBack's interceptorPtr::onBefore
%SsetApiId:[-3]
%SsetServiceType:[1501]
%SaddAnnotation [-1]:[Array
(
%S[0] =&gt; hello
%S[1] =&gt; world
)
]
%Scall MyClass::fnCallBack's interceptorPtr::onEnd
%SaddAnnotation [14]:[helloworld]

%Srequest shutdown