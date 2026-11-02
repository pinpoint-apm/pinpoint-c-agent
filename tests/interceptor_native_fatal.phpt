--TEST--
Suppressed fatal errors still terminate and are not swallowed by an interceptor
--SKIPIF--
<?php if (!extension_loaded('pinpoint_php')) echo 'skip'; ?>
--FILE--
<?php
_pinpoint_join_cut(['trigger_error'], function (...$args) { echo "before\n"; },
    function ($value) { echo "unexpected end\n"; },
    function ($error) { echo "unexpected exception callback\n"; });
error_reporting(0);
trigger_error('must terminate', E_USER_ERROR);
echo "unexpected continuation\n";
?>
--EXPECT--
before
