--TEST--
pinpoint_php internal test case [pinpoint_test_main]
--SKIPIF--
<?php if (!extension_loaded("pinpoint_php")) print "skip"; ?>
--INI--
pinpoint_php.CollectorHost=tcp:localhost:10000
pinpoint_php.SendSpanTimeOutMs=0
pinpoint_php.UnitTest=true
;pinpoint_php._limit for internal use. User do not use it
pinpoint_php._limit=yes
pinpoint_php.DebugReport=true
--FILE--
<?php 
_pinpoint_start_trace();
_pinpoint_add_clue("args",'a,$b');
_pinpoint_add_clue("ret",'a,$b');
_pinpoint_mark_as_error("message you cared","filename you cared",1023);
echo _pinpoint_end_trace()."\n";
--EXPECTF--
[pinpoint] [%d] [%d] [0] pinpoint_start child  [128]
[pinpoint] [%d] [%d] [128] add clue key:args value:a,$b
[pinpoint] [%d] [%d] [128] add clue key:ret value:a,$b
[pinpoint] [%d] [%d]this span:({":E":%d,":FT":1500,":S":%d,"ERR":{"file":"filename you cared","line":1023,"msg":"message you cared"},"args":"a,$b","ret":"a,$b"})
[pinpoint] [%d] [%d]agent try to connect:(localhost:10000)
[pinpoint] [%d] [%d] [128] pinpoint_end_trace Done!
0