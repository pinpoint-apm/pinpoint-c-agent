--TEST--
drop this trace
--SKIPIF--
<?php if (!extension_loaded("pinpoint_php")) print "skip"; ?>
--INI--
pinpoint_php.CollectorHost=unix:/unexist_file.sock
pinpoint_php.SendSpanTimeOutMs=0
pinpoint_php.UnitTest=true
pinpoint_php.DebugReport=true
--FILE--
<?php 
pinpoint_start_trace();
pinpoint_drop_trace();
pinpoint_add_clue("args",'a,$b');
pinpoint_add_clue("ret",'a,$b');
pinpoint_end_trace();
--EXPECTF--
[pinpoint] [%d] [%d]#0 pinpoint_start start
[pinpoint] [%d] [%d]change current#128 status, before:2,now:4
[pinpoint] [%d] [%d]#128 add clue key:args value:a,$b
[pinpoint] [%d] [%d]#128 add clue key:ret value:a,$b
[pinpoint] [%d] [%d]current#128 span dropped,due to TRACE_BLOCK
[pinpoint] [%d] [%d]#128 pinpoint_end_trace Done!