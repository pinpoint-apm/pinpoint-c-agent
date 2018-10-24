--TEST--
Test pinpoint
--INI--
pinpoint_agent.pinpoint_enable=true
pinpoint_agent.trace_exception=true
profiler.proxy.http.header.enable=true
pinpoint_agent.unittest=true

pinpoint.common.TraceLimit = -1
pinpoint.common.SkipTraceTime =5000

--FILE--
<?php

class GetDateInterceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;
    public function __construct()
    {
        $this->apiId = pinpoint_add_api("hello_phpt", 10); // functionName, lineno
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

class QuickStartPlugin extends \Pinpoint\Plugin
{
    public function __construct()
    {
        parent::__construct();
        $p = new GetDateInterceptor();
        $this->addInterceptor($p,"hello_phpt",basename(__FILE__, '.php'));
    }
}

$p = new QuickStartPlugin();
pinpoint_add_plugin($p, basename(__FILE__, '.php'));
pinpint_aop_reload();

function hello_phpt($who)
{
    echo "hello " .$who."\n";
    sleep(1);
    return $who;
}

hello_phpt("phpt, \n how old are you!!!");

hello_phpt("phpt, \n how old are you!!!");

--EXPECTF--
request start
  addInterceptor name:[hello_phpt] class:[test_SkipTraceTime]
  call hello_phpt's interceptorPtr::onBefore
    setApiId:[-2]
    setServiceType:[1501]
    addAnnotation [-1]:[Array
(
    [0] =&gt; phpt, 
 how old are you!!!
)
]
hello phpt, 
 how old are you!!!
  call hello_phpt's interceptorPtr::onEnd
    addAnnotation [%d]:[args:Array
(
    [0] => phpt, 
 how old are you!!!
)
, return:phpt, 
 how old are you!!! ]
  call hello_phpt's interceptorPtr::onBefore
    setApiId:[-2]
    setServiceType:[1501]
    addAnnotation [-1]:[Array
(
    [0] =&gt; phpt, 
 how old are you!!!
)
]
hello phpt, 
 how old are you!!!
  call hello_phpt's interceptorPtr::onEnd
    addAnnotation [%d]:[args:Array
(
    [0] => phpt, 
 how old are you!!!
)
, return:phpt, 
 how old are you!!! ]
request shutdown
This trace had ignored by skiptracetime