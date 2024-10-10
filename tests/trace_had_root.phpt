--TEST--
Test trace_had_root
--SKIPIF--
<?php if (!extension_loaded("pinpoint_php")) print "skip"; ?>
--INI--
pinpoint_php.CollectorHost=unix:/unexist_file.sock
pinpoint_php.SendSpanTimeOutMs=-1
pinpoint_php.UnitTest=true
pinpoint_php.DebugReport=true

--FILE--
<?php
if (_pinpoint_get_trace_depth() == -1){
    echo "case not trace pass \n";
}

_pinpoint_start_trace();
if (_pinpoint_get_trace_depth() == 0){
    echo "case root trace pass \n";
}

_pinpoint_start_trace();
if (_pinpoint_get_trace_depth()== 1){
    echo "case sub trace pass \n";
}
_pinpoint_end_trace();

_pinpoint_end_trace();

?>
--EXPECTF--
case not trace pass 
[pinpoint] [%d:%d] [0] pinpoint_start child [128]
case root trace pass 
[pinpoint] [%d:%d] [128] pinpoint_start child [127]
case sub trace pass 
[pinpoint] [%d:%d] [127] pinpoint_end_trace Done!
[pinpoint] [%d:%d]this span:({":E":%d,":FT":1500,":S":%d,"event":[{":E":%d,":S":%d,":depth":1,":seq":0}]})
[pinpoint] [%d:%d][unittest] current span was dropped
[pinpoint] [%d:%d] [128] pinpoint_end_trace Done!