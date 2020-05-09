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
[pinpoint] [%d] [%d]pinpoint_start start
[pinpoint] [%d] [%d]agent try to connect:(unix:/unexist_file.sock)
[pinpoint] [%d] [%d]connect:(/unexist_file.sock) failed as (No such file or directory)
[pinpoint] [%d] [%d]agent status changed: 2 ->4
[pinpoint] [%d] [%d]key:args value:a,$b
[pinpoint] [%d] [%d]key:ret value:a,$b
[pinpoint] [%d] [%d]pinpoint_end_trace Done!