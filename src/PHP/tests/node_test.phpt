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

$unexist_node = 1024;

pinpoint_start_trace($unexist_node);

pinpoint_add_clue("adf","fdfd",$unexist_node,0);
pinpoint_add_clue("adf","fdfd",$unexist_node,1);
pinpoint_set_context('b','b',$unexist_node);
pinpoint_add_clues(32,"fdfd",$unexist_node,1);
pinpoint_add_clues(32,"fdfd",$unexist_node,0);

pinpoint_get_context('b',$unexist_node);

pinpoint_end_trace($unexist_node);

--EXPECTF--
[pinpoint] [%d] [%d]#1024 pinpoint_start start
[pinpoint] [%d] [%d] start_trace#1024 failed with id is not alive
[pinpoint] [%d] [%d] pinpoint_add_clue#1024 failed. Reason: id is not alive,parameters:adf:fdfd
[pinpoint] [%d] [%d] pinpoint_add_clue#1024 failed. Reason: id is not alive,parameters:adf:fdfd
[pinpoint] [%d] [%d] pinpoint_set_context_key#1024 failed with out_of_range. id is not alive,parameters:b:b
[pinpoint] [%d] [%d] pinpoint_add_clues#1024 failed.Reason id is not alive,parameters:32:fdfd
[pinpoint] [%d] [%d] pinpoint_add_clues#1024 failed.Reason id is not alive,parameters:32:fdfd
[pinpoint] [%d] [%d] pinpoint_get_context_key#1024 failed with id is not alive, parameters:b
[pinpoint] [%d] [%d]#1024 not found