--TEST--
Test pinpoint
--INI--
pinpoint_agent.pinpoint_enable=true
pinpoint_agent.trace_exception=true
profiler.proxy.http.header.enable=true
pinpoint_agent.testCovered=1
pinpoint.common.PPLogLevel = ERROR
--FILE--
<?php
class Func1Interceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;
    public function __construct()
    {
        $this->apiId = pinpoint_add_api("func1", 10); // functionName, lineno
    }

    public function onBefore($callId, $args)
    {
        $trace = pinpoint_get_current_trace();
        pinpoint_log(PINPOINT_DEBUG, "LogTest_debug_onBefore");
        pinpoint_log(PINPOINT_INFO, "LogTest_info_onBefore");
        pinpoint_log(PINPOINT_WARN, "LogTest_warning_onBefore");
        pinpoint_log(PINPOINT_ERROR, "LogTest_error_onBefore");
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
        pinpoint_log(PINPOINT_DEBUG, "LogTest_debug_onEnd");
        pinpoint_log(PINPOINT_INFO, "LogTest_info_onEnd");
        pinpoint_log(PINPOINT_WARN, "LogTest_warning_onEnd");
        pinpoint_log(PINPOINT_ERROR, "LogTest_error_onEnd");
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
        $trace = pinpoint_get_current_trace();
        pinpoint_log(PINPOINT_DEBUG, "LogTest_debug_onException");
        pinpoint_log(PINPOINT_INFO, "LogTest_info_onException");
        pinpoint_log(PINPOINT_WARN, "LogTest_warn_onException");
        pinpoint_log(PINPOINT_ERROR, "LogTest_error_onException");
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

class TestPlugin extends \Pinpoint\Plugin
{
    public function __construct()
    {
        parent::__construct();
        pinpoint_log(PINPOINT_DEBUG, "LogTest_debug_PluginClass");
        pinpoint_log(PINPOINT_INFO, "LogTest_info_PluginClass");
        pinpoint_log(PINPOINT_WARN, "LogTest_warn_PluginClass");
        pinpoint_log(PINPOINT_ERROR, "LogTest_error_PluginClass");
        $i = new Func1Interceptor();
        $this->addInterceptor($i, "func1", basename(__FILE__, '.php'));
    }
}

$p = new TestPlugin();
pinpoint_add_plugin($p, basename(__FILE__, '.php'));
pinpint_aop_reload();

function func1($name)
{
    throw new Exception("I am Exception!");
    return "Hello!".$name;
}

func1("Evy");
?>
--EXPECTF--
request start
%S[ERROR] LogTest_error_PluginClass
  addInterceptor name:[func1] class:[test_PPLogLevel_ERROR]
  call func1's interceptorPtr::onBefore
%S[ERROR] LogTest_error_onBefore
    setApiId:[%i]
    setServiceType:[1501]
    addAnnotation [-1]:[Array
(
    [0] =&gt; Evy
)
]
  call [Func1Interceptor::onexception]
%S[ERROR] LogTest_error_onException
  setExceptionInfo:[Fatal error: I am Exception! in %s]
  [EXCEPTION] file:[%s] line:[%d] msg:[I am Exception!]
  call func1's interceptorPtr::onEnd
%S[ERROR] LogTest_error_onEnd
    addAnnotation [14]:[Array
(
    [0] =&gt; Evy
)
]

Fatal error: Uncaught%sException%sI am Exception!%s
Stack trace:
#0 %s(%d): func1('Evy')
#1 {main}
  thrown in %s on line %d
request shutdown