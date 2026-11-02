--TEST--
Caller argument helper handles a missing caller frame
--SKIPIF--
<?php if (!extension_loaded('pinpoint_php')) echo 'skip'; ?>
--FILE--
<?php
set_error_handler(function ($type, $message) { echo "no function context\n"; return true; });
var_dump(pinpoint_get_caller_arg(0));
restore_error_handler();
?>
--EXPECT--
no function context
bool(false)
