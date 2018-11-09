--TEST--
Test pinpoint
--INI--
pinpoint_agent.pinpoint_enable=true
pinpoint_agent.trace_exception=true
profiler.proxy.http.header.enable=true
pinpoint_agent.testCovered=1

--FILE--
<?php
include "plugins.inc";
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