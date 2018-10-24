--TEST--
Test pinpoint
--INI--
pinpoint_agent.pinpoint_enable=true
pinpoint_agent.trace_exception=true
profiler.proxy.http.header.enable=true
pinpoint_agent.unittest=true

--FILE--
<?php

class QuickStartPlugin extends \Pinpoint\Plugin
{
    public function __construct()
    {
        parent::__construct();
        $this->addSimpleInterceptor("hello", -1);
    }
}

$p = new QuickStartPlugin();
pinpoint_add_plugin($p, basename(__FILE__, '.php'));
pinpint_aop_reload();


function hello($who)
{
    echo "hello " .$who."\n";
    return $who;
}
hello("pinpoint");
?>
--EXPECTF--
request start
  addSimpleInterceptor name:[hello]
  call hello's interceptorPtr::onBefore
    SimpleInterceptor->addAnnotation key:[%s] value:[{[0]:pinpoint }] 
hello pinpoint
  call hello's interceptorPtr::onEnd
    SimpleInterceptor->addAnnotation key:[%s] value:[pinpoint] 
request shutdown