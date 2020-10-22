--TEST--
Check  call statck
--SKIPIF--
<?php if (!extension_loaded("pinpoint_php")) print "skip"; ?>
--INI--
pinpoint_php.CollectorHost=unix:/unexist_file.sock
pinpoint_php.SendSpanTimeOutMs=200
pinpoint_php.UnitTest=true
;pinpoint_php._limit for internal use. User do not use it
pinpoint_php._limit=yes
pinpoint_php.DebugReport=true
--FILE--
<?php 


pinpoint_start_trace();

pinpoint_set_context('a','a');
var_dump(pinpoint_get_context('a'));
pinpoint_set_context('b','b');
var_dump(pinpoint_get_context('b'));

pinpoint_set_context('c','c');
var_dump(pinpoint_get_context('c'));

var_dump(pinpoint_get_context('not exsit'));

pinpoint_end_trace();

$id = pinpoint_start_trace(0);
$id = pinpoint_start_trace($id);
pinpoint_set_context('a','a',$id);
pinpoint_set_context('b','b',$id);
pinpoint_set_context('c','c',$id);

var_dump(pinpoint_get_context('c',$id));
var_dump(pinpoint_get_context('b',$id));
var_dump(pinpoint_get_context('a',$id));
var_dump(pinpoint_get_context('not exsit',$id));

$id = pinpoint_end_trace($id);
pinpoint_end_trace($id);

?>
--EXPECTF--
[pinpoint] [%d] [%d]#0 pinpoint_start start
string(1) "a"
string(1) "b"
string(1) "c"
[pinpoint] [%d] [%d] pinpoint_get_context_key#128 failed with map::at, parameters:not exsit
bool(false)
[pinpoint] [%d] [%d]this span:({"E":0,"FT":1500,"S":%d})
[pinpoint] [%d] [%d]agent try to connect:(unix:/unexist_file.sock)
[pinpoint] [%d] [%d]connect:(/unexist_file.sock) failed as (No such file or directory)
[pinpoint] [%d] [%d]#128 pinpoint_end_trace Done!
[pinpoint] [%d] [%d]#0 pinpoint_start start
[pinpoint] [%d] [%d]#128 pinpoint_start start
string(1) "c"
string(1) "b"
string(1) "a"
[pinpoint] [%d] [%d] pinpoint_get_context_key#127 failed with map::at, parameters:not exsit
bool(false)
[pinpoint] [%d] [%d]#127 pinpoint_end_trace Done!
[pinpoint] [%d] [%d]this span:({"E":0,"FT":1500,"S":%d,"calls":[{"E":0,"S":0}]})
[pinpoint] [%d] [%d]agent try to connect:(unix:/unexist_file.sock)
[pinpoint] [%d] [%d]connect:(/unexist_file.sock) failed as (No such file or directory)
[pinpoint] [%d] [%d]#128 pinpoint_end_trace Done!