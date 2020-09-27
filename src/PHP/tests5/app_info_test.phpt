--TEST--
app info 
--SKIPIF--
<?php if (!extension_loaded("pinpoint_php")) print "skip"; ?>
--INI--
pinpoint_php.CollectorHost=unix:/unexist_file.sock
pinpoint_php.SendSpanTimeOutMs=0
pinpoint_php.UnitTest=true
pinpoint_php.DebugReport=true
--FILE--
<?php 
var_dump(pinpoint_start_time());
var_dump("APP".'^'.strval(pinpoint_start_time()).'^'.strval(pinpoint_unique_id()));
--EXPECTF--
int(%d)
string(%d) "APP^%d^%d"