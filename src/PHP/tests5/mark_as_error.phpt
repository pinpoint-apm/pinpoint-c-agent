--TEST--
pinpioint_php internal test case [pinpoint_test_main]
--SKIPIF--
<?php if (!extension_loaded("pinpoint_php")) print "skip"; ?>
--INI--
pinpoint_php.CollectorHost=unix:/unexist_file.sock
pinpoint_php.SendSpanTimeOutMs=0
pinpoint_php.UnitTest=true
;pinpoint_php._limit for internal use. User do not use it
pinpoint_php._limit=yes
pinpoint_php.DebugReport=true
--FILE--
<?php 
pinpoint_start_trace();
pinpoint_add_clue("args",'a,$b');
pinpoint_add_clue("ret",'a,$b');
pinpoint_mark_as_error("message you cared","filename you cared",1023);
echo pinpoint_end_trace()."\n";
--EXPECTF--
[pinpoint] [%d] [%d]#0 pinpoint_start child #128
[pinpoint] [%d] [%d]#128 add clue key:args value:a,$b
[pinpoint] [%d] [%d]#128 add clue key:ret value:a,$b
[pinpoint] [%d] [%d]this span:({"E":%d,"ERR":{"file":"filename you cared","line":1023,"msg":"message you cared"},"FT":%d,"S":%d,"args":"a,$b","ret":"a,$b"})
[pinpoint] [%d] [%d]agent try to connect:(unix:/unexist_file.sock)
[pinpoint] [%d] [%d]connect:(/unexist_file.sock) failed as (No such file or directory)
[pinpoint] [%d] [%d]#128 pinpoint_end_trace Done!
0