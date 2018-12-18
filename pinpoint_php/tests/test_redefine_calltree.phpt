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
    pinpoint_start_calltree();
    $trace = pinpoint_get_current_trace();
    if ($trace)
    {
      $event = $trace->traceBlockBegin($callId);
      $event->markBeforeTime();
      $event->setApiId($this->apiId);
      $event->setServiceType(PINPOINT_PHP_RPC_TYPE);
      $event->addAnnotation(PINPOINT_ANNOTATION_ARGS, htmlspecialchars(print_r($args,true),ENT_QUOTES));
    }
    pinpoint_start_calltree();
    pinpoint_start_calltree();
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

    pinpoint_end_calltree();
    pinpoint_end_calltree();
    pinpoint_end_calltree();
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
        $this->addSimpleInterceptor("hello", -1);
        $i = new onCallTreeInterceptor();
        $this->addInterceptor($i,"heComes",basename(__FILE__));
    }
};

$p = new QuickStartPlugin();
pinpoint_add_plugin($p, basename(__FILE__, '.php'));
pinpint_aop_reload();

function hello($who)
{
    echo "hello " .$who."\n";
    return $who;
}

/// hello should not be captured

hello("Should not show");

function heComes($who)
{
    return "ghost: " .$who ."comes\n";
}

heComes("Michael Joseph Jackson");



--EXPECTF--
request start
addSimpleInterceptor name:[hello]
addInterceptor name:[heComes] class:[test_redefine_calltree.php]
call hello's interceptorPtr::onBefore
hello Should not show
call hello's interceptorPtr::onEnd
call heComes's interceptorPtr::onBefore
  setApiId:[%i]
  setServiceType:[1501]
  addAnnotation [-1]:[Array
(
    [0] =&gt; Michael Joseph Jackson
)
]
call heComes's interceptorPtr::onEnd
  addAnnotation [14]:[ghost: Michael Joseph Jacksoncomes
]
  request shutdown