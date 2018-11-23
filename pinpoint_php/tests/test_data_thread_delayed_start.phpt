--TEST--
Test data thread reload  
--INI--
pinpoint_agent.pinpoint_enable=true
pinpoint_agent.trace_exception=true
profiler.proxy.http.header.enable=true
pinpoint_agent.testCovered=4
pinpoint.common.CollectorSpanIp=127.0.0.1
pinpoint.common.CollectorSpanPort=79
pinpoint.common.CollectorStatIp=127.0.0.1
pinpoint.common.CollectorStatPort=79
pinpoint.common.CollectorTcpIp=127.0.0.1
pinpoint.common.CollectorTcpPort=79
pinpoint.common.RedefineAgentMain=ttt
--FILE--
<?php
include "plugins.inc";

function ttt(){
    echo "ttt";
}

function hello($who)
{
    echo "hello " .$who."\n";
    return $who;
}
ttt();
hello("pinpoint");

?>
--EXPECTF--
request start
  addSimpleInterceptor name:[hello]
  call ttt's interceptorPtr::onBefore
    data proxy thread start
ttt  call ttt's interceptorPtr::onEnd
  call hello's interceptorPtr::onBefore
hello pinpoint
  call hello's interceptorPtr::onEnd
request shutdown