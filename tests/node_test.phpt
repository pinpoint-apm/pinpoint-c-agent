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

_pinpoint_start_trace($unexist_node);

_pinpoint_add_clue("adf","fdfd",$unexist_node,0);
_pinpoint_add_clue("adf","fdfd",$unexist_node,1);
_pinpoint_set_context('b','b',$unexist_node);
_pinpoint_add_clues(32,"fdfd",$unexist_node,1);
_pinpoint_add_clues(32,"fdfd",$unexist_node,0);

_pinpoint_get_context('b',$unexist_node);

_pinpoint_end_trace($unexist_node);

--EXPECTF--
[pinpoint] [%d] [%d] start_trace [1024] failed with #1024 is not alive
[pinpoint] [%d] [%d] pinpoint_add_clue [1024] failed. Reason: #1024 is not alive,parameters:adf:fdfd
[pinpoint] [%d] [%d] pinpoint_add_clue [1024] failed. Reason: #1024 is not alive,parameters:adf:fdfd
[pinpoint] [%d] [%d] pinpoint_set_context_key [1024] failed with out_of_range. #1024 is not alive,parameters:b:b
[pinpoint] [%d] [%d] pinpoint_add_clues [1024] failed. Reason #1024 is not alive,parameters:32:fdfd
[pinpoint] [%d] [%d] pinpoint_add_clues [1024] failed. Reason #1024 is not alive,parameters:32:fdfd
[pinpoint] [%d] [%d] pinpoint_get_context_key [1024] failed with #1024 is not alive, parameters:b
[pinpoint] [%d] [%d]end_trace 1024 out_of_range exception: #1024 is not alive