--TEST--
Check tracelimit
--SKIPIF--
<?php if (!extension_loaded("pinpoint_php")) print "skip"; ?>
--INI--
pinpoint_php.CollectorHost=tcp:localhost:10000
pinpoint_php.SendSpanTimeOutMs=200
pinpoint_php.UnitTest=true
pinpoint_php.TraceLimit=2
;pinpoint_php._limit for internal use. User do not use it
pinpoint_php._limit=yes
pinpoint_php.DebugReport=true
--FILE--
<?php 
if(_pinpoint_trace_limit() || _pinpoint_trace_limit() ||_pinpoint_trace_limit() || _pinpoint_trace_limit())
{
    echo "pass";
}else{
    echo "_pinpoint_trace_limit failed";
}
--EXPECTF--
[pinpoint] [%d] [%d]This span dropped. max_trace_limit:2 current_tick:2 onLine:0
pass