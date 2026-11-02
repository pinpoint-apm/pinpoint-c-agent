--TEST--
Interceptor preserves engine exit without notifying ordinary exception callbacks
--SKIPIF--
<?php
if (!extension_loaded('pinpoint_php')) echo 'skip';
if (PHP_VERSION_ID < 80400) echo 'skip exit becomes a function in PHP 8.4';
?>
--FILE--
<?php
_pinpoint_join_cut(['exit'], function (...$args) { echo "before\n"; },
    function ($value) { echo "unexpected end\n"; },
    function ($error) { echo "unexpected exception callback\n"; });
exit(7);
echo "unexpected continuation\n";
?>
--EXPECT--
before
