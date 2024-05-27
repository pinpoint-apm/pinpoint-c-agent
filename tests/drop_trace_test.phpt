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
_pinpoint_start_trace();
_pinpoint_drop_trace();
_pinpoint_add_clue("args",'a,$b');
_pinpoint_add_clue("ret",'a,$b');
_pinpoint_end_trace();
--EXPECTF--
[pinpoint] [%d] [%d] [0] pinpoint_start child  [128]
[pinpoint] [%d] [%d]change current [128] status, before:2,now:4
[pinpoint] [%d] [%d] [128] add clue key:args value:a,$b
[pinpoint] [%d] [%d] [128] add clue key:ret value:a,$b
[pinpoint] [%d] [%d]current [128] span dropped,due to TRACE_BLOCK
[pinpoint] [%d] [%d] [128] pinpoint_end_trace Done!