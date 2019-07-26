--TEST--
Test pinpoint_unique_id
--SKIPIF--
<?php if (!extension_loaded("pinpoint_php")) print "skip"; ?>
--INI--
pinpoint_php.CollectorHost=unix:/unexist_file.sock
pinpoint_php.SendSpanTimeOutMs=0
pinpoint_php.UnitTest=true
--FILE--
<?php
 
$v1= pinpoint_unique_id();
$v2= pinpoint_unique_id();
$v3= pinpoint_unique_id();
var_dump( $v2*2 === $v1+$v3 );

--EXPECTF--
bool(true)