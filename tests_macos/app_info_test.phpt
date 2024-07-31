--TEST--
app info 
--SKIPIF--
<?php if (!extension_loaded("pinpoint_php")) print "skip"; ?>
--INI--
pinpoint_php.CollectorHost=tcp:localhost:10000
pinpoint_php.SendSpanTimeOutMs=0
pinpoint_php.UnitTest=true
pinpoint_php.DebugReport=true
--FILE--
<?php 
var_dump(_pinpoint_start_time());
var_dump("APP".'^'.strval(_pinpoint_start_time()).'^'.strval(_pinpoint_unique_id()));
--EXPECTF--
float(%d)
string(%d) "APP^%d^%d"