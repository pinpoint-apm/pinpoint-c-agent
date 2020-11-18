--TEST--
Test pinpoint_unique_id
--SKIPIF--
<?php if (!extension_loaded("pinpoint_php")) print "skip"; ?>
--INI--
pinpoint_php.CollectorHost=unix:/unexist_file.sock
pinpoint_php.SendSpanTimeOutMs=0
pinpoint_php.UnitTest=true
pinpoint_php.DebugReport=true
--FILE--
<?php
$id = pinpoint_start_trace(0);
pinpoint_add_clue("name","Foo",$id);
$id = pinpoint_start_trace($id);
pinpoint_add_clues("global","global_value",$id,1);
pinpoint_add_clue("global","global_value",$id,1);
pinpoint_add_clues("name","Foo1",$id);
$id = pinpoint_start_trace($id);
pinpoint_add_clue("name","Foo2",$id);
$id = pinpoint_end_trace($id);
$id = pinpoint_end_trace($id);
$id = pinpoint_end_trace($id);
--EXPECTF--
[pinpoint] [%d] [%d]#0 pinpoint_start start
[pinpoint] [%d] [%d]#128 add clue key:name value:Foo
[pinpoint] [%d] [%d]#128 pinpoint_start start
[pinpoint] [%d] [%d]#127 add clues:global:global_value
[pinpoint] [%d] [%d]#127 add clue key:global value:global_value
[pinpoint] [%d] [%d]#127 add clues:name:Foo1
[pinpoint] [%d] [%d]#127 pinpoint_start start
[pinpoint] [%d] [%d]#126 add clue key:name value:Foo2
[pinpoint] [%d] [%d]#126 pinpoint_end_trace Done!
[pinpoint] [%d] [%d]#127 pinpoint_end_trace Done!
[pinpoint] [%d] [%d]this span:({"E":%d,"FT":1500,"S":%d,"calls":[{"E":%d,"S":%d,"calls":[{"E":%d,"S":%d,"name":"Foo2"}],"clues":["name:Foo1"]}],"clues":["global:global_value"],"global":"global_value","name":"Foo"})
[pinpoint] [%d] [%d]agent try to connect:(unix:/unexist_file.sock)
[pinpoint] [%d] [%d]connect:(/unexist_file.sock) failed as (No such file or directory)
[pinpoint] [%d] [%d]#128 pinpoint_end_trace Done!