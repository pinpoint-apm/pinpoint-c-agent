--TEST--
Check tracelimit
--SKIPIF--
<?php if (!extension_loaded("pinpoint_php")) print "skip"; ?>
--INI--
pinpoint_php.CollectorHost=tcp:localhost:10000
pinpoint_php.SendSpanTimeOutMs=200
pinpoint_php.UnitTest=true
pinpoint_php.TraceLimit=0
;pinpoint_php._limit for internal use. does not use it
pinpoint_php._limit=yes
pinpoint_php.DebugReport=true
--FILE--
<?php

$id = _pinpoint_start_trace(0);
$id = _pinpoint_start_trace($id);
_pinpoint_set_context('a','a',$id);
_pinpoint_set_context('b','b',$id);
_pinpoint_set_context('c','c',$id);

var_dump(_pinpoint_get_context('c',$id));
var_dump(_pinpoint_get_context('b',$id));
var_dump(_pinpoint_get_context('a',$id));
var_dump(_pinpoint_get_context('not exist',$id));

$id = _pinpoint_end_trace($id);

if(_pinpoint_trace_limit())
{
   _pinpoint_drop_trace($id);
}

_pinpoint_end_trace($id);

--EXPECTF--
[pinpoint] [%d] [%d] [0] pinpoint_start child  [128]
[pinpoint] [%d] [%d] [128] pinpoint_start child  [127]
string(1) "c"
string(1) "b"
string(1) "a"
[pinpoint] [%d] [%d] pinpoint_get_context_key [127] failed with invalid map<K, T> key, parameters:not exist
bool(false)
[pinpoint] [%d] [%d] [127] pinpoint_end_trace Done!
[pinpoint] [%d] [%d]This span dropped. max_trace_limit:0 current_tick:0 onLine:0
[pinpoint] [%d] [%d]change current [128] status, before:2,now:4
[pinpoint] [%d] [%d]current [128] span dropped,due to TRACE_BLOCK
[pinpoint] [%d] [%d] [128] pinpoint_end_trace Done!