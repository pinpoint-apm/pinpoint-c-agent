--TEST--
Test pinpoint
--INI--
pinpoint_agent.pinpoint_enable=true
pinpoint_agent.trace_exception=true
profiler.proxy.http.header.enable=true
pinpoint_agent.testCovered=2
pinpoint.common.CollectorSpanIp=127.0.0.1
pinpoint.common.CollectorSpanPort=79
pinpoint.common.CollectorStatIp=127.0.0.1
pinpoint.common.CollectorStatPort=79
pinpoint.common.CollectorTcpIp=127.0.0.1
pinpoint.common.CollectorTcpPort=79
pinpoint.common.ReconTimeOut=1
pinpoint.common.PPLogLevel = WARN
--FILE--
<?php
include "plugins.inc";
function hello($who)
{
    echo "hello " .$who."\n";
    return $who;
}
hello("pinpoint");
sleep(3);
?>
--EXPECTF--
data proxy thread start
request start
  addSimpleInterceptor name:[hello]
  call hello's interceptorPtr::onBefore
    SimpleInterceptor->addAnnotation key:[-1] value:[{[0]:pinpoint }] 
hello pinpoint
  call hello's interceptorPtr::onEnd
    SimpleInterceptor->addAnnotation key:[14] value:[pinpoint] 
%s %s [pinpoint] [%s] pinpoint_client.cpp:%d [WARN] PinpointClient connect timeout expiredTime:[1]
 %s %s [pinpoint] [%s] pinpoint_client.cpp:%d [WARN] PinpointClient connect timeout expiredTime:[1]
 %s %s [pinpoint] [%s] pinpoint_client.cpp:%d [WARN] PinpointClient connect timeout expiredTime:[1]
 request shutdown

%s %s [pinpoint] [%s] executor.cpp:%d [WARN] background task finished