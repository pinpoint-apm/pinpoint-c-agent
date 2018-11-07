--TEST--
Test pinpoint
--INI--
pinpoint_agent.trace_exception=true
pinpoint_agent.config_full_name=/home/liumingyi/git/pinpoint-c-agent/simulate/tmp/pinpoint_agent.conf
profiler.proxy.http.header.enable=true
pinpoint_agent.unittest=true
//pinpoint_agent.pluginsRootPath=php-plugins
//pinpoint_agent.entryFilename=plugins_create.php
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
    return $who;
}
hello_phpt("phpt, \n how old are you!!!");

--EXPECTF--
request start
  addInterceptor name:hello_phpt class:hello_phpt
  call hello_phpt's interceptorPtr::onBefore
    setApiId:-28
    setServiceType:1501
    addAnnotation -1:Array
(
    [0] =&gt; phpt, 
 how old are you!!!
)

hello phpt, 
 how old are you!!!
  call hello_phpt's interceptorPtr::onEnd
    addAnnotation 14:args:Array
(
    [0] => phpt, 
 how old are you!!!
)
, return:phpt, 
 how old are you!!! 
request shutdown