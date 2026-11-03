--TEST--
Interceptor rejects user code and resolves inherited methods with long class names
--SKIPIF--
<?php if (!extension_loaded('pinpoint_php')) echo 'skip'; ?>
--FILE--
<?php
$before = function (...$args) { echo "before\n"; };
$end = function ($value) { echo "end\n"; };
$exception = function ($error) { echo "unexpected exception\n"; };
function native_review_target() { return 'user function works'; }
class NativeReviewTarget {
    public function run() { return 'user method works'; }
}
set_error_handler(function ($type, $message) { echo "rejected user code\n"; return true; });
_pinpoint_join_cut(['native_review_target'], $before, $end, $exception);
_pinpoint_join_cut(['NativeReviewTarget', 'run'], $before, $end, $exception);
restore_error_handler();
echo native_review_target(), "\n";
echo (new NativeReviewTarget())->run(), "\n";

$class = 'NativeReview' . str_repeat('LongName', 20);
eval('class ' . $class . ' extends DateTime {}');
_pinpoint_join_cut([$class, 'format'], $before, $end, $exception);
// Registering the same inherited function twice must not wrap our own handler.
_pinpoint_join_cut([$class, 'format'], $before, $end, $exception);
_pinpoint_join_cut(['DateTime', 'format'],
    function (...$args) { echo "parent before\n"; },
    function ($value) { echo "parent end\n"; }, $exception);
$date = new $class('2026-01-01');
echo $date->format('Y'), "\n";
echo (new DateTime('2027-01-01'))->format('Y'), "\n";
// A class declared after its parent was hooked inherits the parent's handler.
eval('class NativeLateDate extends DateTime {}');
echo (new NativeLateDate('2028-01-01'))->format('Y'), "\n";
?>
--EXPECT--
rejected user code
rejected user code
user function works
user method works
before
end
2026
parent before
parent end
2027
parent before
parent end
2028
