--TEST--
Test redefine calltree
--INI--
pinpoint_agent.pinpoint_enable=true
pinpoint_agent.trace_exception=true
profiler.proxy.http.header.enable=true
pinpoint_agent.testCovered=1
pinpoint.common.DefineCallTree=true

--FILE--
<?php

class onCallTreeInterceptor extends \Pinpoint\Interceptor
{
  var $apiId = -1;
  public function __construct()
  {
    $this->apiId = pinpoint_add_api("heComes", 10); // functionName, lineno
  }

  public function onBefore($callId, $args)
  {
    pinpoint_start_calltrace();
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
      $retArgs = $data["result"];
      $event = $trace->getEvent($callId);
      if ($event)
      {
        $event->addAnnotation(PINPOINT_ANNOTATION_RETURN, htmlspecialchars(print_r($retArgs,true),ENT_QUOTES));
        $event->markAfterTime();
        $trace->traceBlockEnd($event);
      }
    }

    pinpoint_end_calltrace();
  }

  public function onException($callId, $exceptionStr)
  {

  }
};




class QuickStartPlugin extends \Pinpoint\Plugin
{
    public function __construct()
    {
        parent::__construct();
        $i = new onCallTreeInterceptor();
        $this->addInterceptor($i,"heComes",basename(__FILE__));
    }
};

$p = new QuickStartPlugin();
pinpoint_add_plugin($p, basename(__FILE__, '.php'));
pinpint_aop_reload();

$_SERVER[PINPOINT_SAMPLE_HTTP_HEADER] = PINPOINT_SAMPLE_FALSE;

function heComes($who)
{
    return "ghost: " .$who ."comes\n";
}

heComes("Michael Joseph Jackson");


// function getallheaders_t() 
// { 
//        $headers = []; 
//    foreach ($_SERVER as $name => $value) 
//    { 
//        if (substr($name, 0, 5) == 'HTTP_') 
//        { 
//            $headers[str_replace(' ', '-', ucwords(strtolower(str_replace('_', ' ', substr($name, 5)))))] = $value; 
//        } 
//    } 
//    return $headers; 
// } 

// print_r($_SERVER);

--EXPECTF--
request start
addInterceptor name:[heComes] class:[test_no_sample.php]
call heComes's interceptorPtr::onBefore
  This trace had ignored by TraceLimit
call heComes's interceptorPtr::onEnd
request shutdown