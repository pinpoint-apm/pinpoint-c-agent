--TEST--
Check tracelimit
--SKIPIF--
<?php if (!extension_loaded("pinpoint_php")) print "skip"; ?>
--INI--
pinpoint_php.CollectorHost=unix:/unexist_file.sock
pinpoint_php.SendSpanTimeOutMs=200
pinpoint_php.UnitTest=true
pinpoint_php.TraceLimit=2
;pinpoint_php._limit for internal use. User do not use it
pinpoint_php._limit=yes
pinpoint_php.DebugReport=true
--FILE--
<?php 
if(pinpoint_tracelimit() || pinpoint_tracelimit() ||pinpoint_tracelimit() || pinpoint_tracelimit())
{
    echo "pass";
}else{
    echo "pinpoint_tracelimit failed";
}
--EXPECTF--
[pinpoint] [%d] [%d]This span dropped. max_trace_limit:2 current_tick:2 onLine:0
pass