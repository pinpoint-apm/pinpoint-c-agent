--TEST--
Test pinpoint
--INI--
pinpoint_agent.pinpoint_enable=true
pinpoint_agent.trace_exception=true
profiler.proxy.http.header.enable=true
pinpoint_agent.testCovered=1

--FILE--
<?php

class TestFunc2Interceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;
    public function __construct()
    {
        $this->apiId = pinpoint_add_api("testNameSpace\\test_func2", 10); // functionName, lineno
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
            if ($self)
            {
                $event->addAnnotation(PINPOINT_ANNOTATION_ARGS,
                    sprintf("[ %s ] \n this.num=%d ",htmlspecialchars(print_r($args,true),ENT_QUOTES),$self->num));
            }else{
                $event->addAnnotation(PINPOINT_ANNOTATION_ARGS, htmlspecialchars(print_r($args,true),ENT_QUOTES));
            }

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


class TestMethodInterceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;
    public function __construct()
    {
        $this->apiId = pinpoint_add_api("test_method", 10); // functionName, lineno
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
            if ($self)
            {
                $event->addAnnotation(PINPOINT_ANNOTATION_ARGS,
                    sprintf("[ %s ] \n this.num=%d ",htmlspecialchars(print_r($args,true),ENT_QUOTES),$self->num));
            }else{
                $event->addAnnotation(PINPOINT_ANNOTATION_ARGS, htmlspecialchars(print_r($args,true),ENT_QUOTES));
            }

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

        $i = new TestFunc2Interceptor();
        $this->addInterceptor($i, "testNameSpace\\test_func2", basename(__FILE__, '.php'));

        $i = new TestMethodInterceptor();
        $this->addInterceptor($i, "testNameSpace\\TestClass::getVarAdd", basename(__FILE__, '.php'));

    }
}

$p = new QuickStartPlugin();
pinpoint_add_plugin($p, basename(__FILE__, '.php'));
pinpint_aop_reload();

include("test_namespace_demo.php");
echo testNameSpace\test_func2("the one", 22);
$t = new testNameSpace\TestClass();
$t->num = 80;
echo $t->getVarAdd(8);
?>

--EXPECTF--
%Srequest start
%SaddInterceptor name:[testNameSpace\test_func2] class:[test_namespace]
%SaddInterceptor name:[testNameSpace\TestClass::getVarAdd] class:[test_namespace]
%Scall testNameSpace\test_func2's interceptorPtr::onBefore
%SsetApiId:[-2]
%SsetServiceType:[1501]
%SaddAnnotation [-1]:[Array
%S(
%S[0] =&gt; the one
%S[1] =&gt; 22
%S)
%S]
%Scall testNameSpace\test_func2's interceptorPtr::onEnd
%SaddAnnotation [14]:[this is test_func2 in namespace testNameSpace: arg1=the one, arg2=22]
%Sthis is test_func2 in namespace testNameSpace: arg1=the one, arg2=22  call testNameSpace\TestClass::getVarAdd's interceptorPtr::onBefore
%SsetApiId:[-3]
%SsetServiceType:[1501]
%SaddAnnotation [-1]:[[ Array
%S(
%S[0] =&gt; 8
%S)
%S] 
%Sthis.num=80 ]
%Scall testNameSpace\TestClass::getVarAdd's interceptorPtr::onEnd
%SaddAnnotation [14]:[88]
%S88
%Srequest shutdown