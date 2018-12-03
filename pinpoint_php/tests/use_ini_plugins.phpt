--TEST--
Test ini Plugin
--INI--
pinpoint_agent.pinpoint_enable=true
pinpoint_agent.trace_exception=true
profiler.proxy.http.header.enable=true
pinpoint_agent.testCovered=1
pinpoint_agent.entryFilename=plugins.inc
pinpoint_agent.pluginsRootPath=./tests/

--FILE--
<?php
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
    SimpleInterceptor->addAnnotation key:[-1] value:[{[0]:pinpoint }] 
hello pinpoint
  call hello's interceptorPtr::onEnd
    SimpleInterceptor->addAnnotation key:[14] value:[pinpoint] 
request shutdown