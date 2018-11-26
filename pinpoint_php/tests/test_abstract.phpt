--TEST--
Test pinpoint
--INI--
pinpoint_agent.pinpoint_enable=true
pinpoint_agent.trace_exception=true
profiler.proxy.http.header.enable=true
pinpoint_agent.testCovered=1

--FILE--
<?php

class TestAbstract1Interceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;
    var $save_event;
//    public function TestFunc1Interceptor()
    function __construct()
    {
        $this->apiId = pinpoint_add_api("Student::drink", 32); // functionName, lineno
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

class TestAbstract2Interceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;
    var $save_event;
//    public function TestFunc1Interceptor()
    function __construct()
    {
        $this->apiId = pinpoint_add_api("Person::breath", 32); // functionName, lineno
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

class QuickStartPlugin extends \Pinpoint\Plugin
{
    public function __construct()
    {
        parent::__construct();
        $i = new TestAbstract1Interceptor();
        $this->addInterceptor($i, "Student::drink", basename(__FILE__, '.php'));

        $i = new TestAbstract2Interceptor();
        $this->addInterceptor($i, "Person::breath", basename(__FILE__, '.php'));
    }
}

$p = new QuickStartPlugin();
pinpoint_add_plugin($p, basename(__FILE__, '.php'));
pinpint_aop_reload();

abstract class Person{
    protected $name;

    abstract function eat();
    abstract function drink();

    function breath(){
        return $this->name." is alive.<br/>";
    }
}

class Student extends Person{
    protected $name;

    function __construct($name)
    {
        $this->name = $name;
    }

    function eat(){
        return $this->name." is eating.<br/>";
    }

    function drink(){
        return $this->name." is drinking.<br/>";
    }
}

$stu = new Student("sam");
echo $stu->eat();
echo $stu->drink();
echo $stu->breath();
?>

--EXPECTF--
%Srequest start
%SaddInterceptor name:[Student::drink] class:[test_abstract]
%SaddInterceptor name:[Person::breath] class:[test_abstract]
%Ssam is eating.<br/>  call Student::drink's interceptorPtr::onBefore
%SsetApiId:[%s]
%SsetServiceType:[1501]
%SaddAnnotation [-1]:[Array
%S(
%S)
%S]
%Scall Student::drink's interceptorPtr::onEnd
%SaddAnnotation [14]:[sam is drinking.&lt;br/&gt;]
%Ssam is drinking.<br/>  call Person::breath's interceptorPtr::onBefore
%SsetApiId:[%s]
%SsetServiceType:[1501]
%SaddAnnotation [-1]:[Array
%S(
%S)
%S]
%Scall Person::breath's interceptorPtr::onEnd
%SaddAnnotation [14]:[sam is alive.&lt;br/&gt;]
%Ssam is alive.<br/>
%Srequest shutdown