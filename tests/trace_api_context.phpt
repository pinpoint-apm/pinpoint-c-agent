--TEST--
Check  call statck
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

pinpoint_set_context('a','a');
var_dump(pinpoint_get_context('a'));
pinpoint_set_context('b','b');
var_dump(pinpoint_get_context('b'));

pinpoint_set_context('c','c');
var_dump(pinpoint_get_context('c'));

var_dump(pinpoint_get_context('not exist'));

pinpoint_end_trace();

$id = pinpoint_start_trace(0);
$id = pinpoint_start_trace($id);
pinpoint_set_context('a','a',$id);
pinpoint_set_context('b','b',$id);
pinpoint_set_context('c','c',$id);

var_dump(pinpoint_get_context('c',$id));
var_dump(pinpoint_get_context('b',$id));
var_dump(pinpoint_get_context('a',$id));
var_dump(pinpoint_get_context('not exist',$id));

$id = pinpoint_end_trace($id);
pinpoint_end_trace($id);
pinpoint_status();
?>
--EXPECTF--
[pinpoint] [%d] [%d] [0] pinpoint_start child  [128]
string(1) "a"
string(1) "b"
string(1) "c"
[pinpoint] [%d] [%d] pinpoint_get_context_key [128] failed with map::at, parameters:not exist
bool(false)
[pinpoint] [%d] [%d]this span:({":E":%d,":FT":1500,":S":%d})
[pinpoint] [%d] [%d]agent try to connect:(:/unexist_file.sock)
[pinpoint] [%d] [%d]connect:(:/unexist_file.sock) failed error_code: 2
[pinpoint] [%d] [%d] [128] pinpoint_end_trace Done!
[pinpoint] [%d] [%d] [0] pinpoint_start child  [128]
[pinpoint] [%d] [%d] [128] pinpoint_start child  [127]
string(1) "c"
string(1) "b"
string(1) "a"
[pinpoint] [%d] [%d] pinpoint_get_context_key [127] failed with map::at, parameters:not exist
bool(false)
[pinpoint] [%d] [%d] [127] pinpoint_end_trace Done!
[pinpoint] [%d] [%d]this span:({":E":%d,":FT":1500,":S":%d,"calls":[{":E":%d,":S":%d}]})
[pinpoint] [%d] [%d] [128] pinpoint_end_trace Done!
{
	"common_libary_version" : "0.5.0",
	"pool_free_node" : 128,
	"pool_total_node" : 128
}