--TEST--
Interceptor reports PHP exceptions and does not execute the original after before throws
--SKIPIF--
<?php if (!extension_loaded('pinpoint_php')) echo 'skip'; ?>
--FILE--
<?php
_pinpoint_join_cut(['intdiv'], function (...$args) {},
    function ($value) { echo "unexpected end\n"; },
    function ($error) { echo 'notified:', get_class($error), "\n"; });
try {
    intdiv(1, 0);
} catch (Throwable $error) {
    echo 'caught:', get_class($error), "\n";
}
$original = new RuntimeException('before failed');
$path = __DIR__ . '/pinpoint-before-side-effect.txt';
@unlink($path);
_pinpoint_join_cut(['file_put_contents'],
    function (...$args) use ($original) { throw $original; },
    function ($value) { echo "unexpected end\n"; },
    function ($error) {
        echo 'notified:', get_class($error), "\n";
        throw new LogicException('notification failed');
    });
try {
    file_put_contents($path, 'must not be written');
} catch (Throwable $error) {
    var_dump($error === $original);
    echo 'notification error:', get_class($error->getPrevious()), "\n";
}
echo 'file exists:', file_exists($path) ? 'yes' : 'no', "\n";
?>
--CLEAN--
<?php @unlink(__DIR__ . '/pinpoint-before-side-effect.txt'); ?>
--EXPECT--
notified:DivisionByZeroError
caught:DivisionByZeroError
notified:RuntimeException
bool(true)
notification error:LogicException
file exists:no
