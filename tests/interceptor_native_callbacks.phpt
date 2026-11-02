--TEST--
Interceptor preserves object and static callback scope
--SKIPIF--
<?php if (!extension_loaded('pinpoint_php')) echo 'skip'; ?>
--FILE--
<?php
class NativeCallbacks {
    private $label = 'object';
    public function before($value) {
        echo $this->label, ':before:', $value, "\n";
        return ['changed'];
    }
    public static function end($value) {
        echo self::class, ':end:', $value, "\n";
    }
    public function exception($error) {
        echo 'unexpected exception', "\n";
    }
}
$callbacks = new NativeCallbacks();
_pinpoint_join_cut(['strrev'], [$callbacks, 'before'],
    ['NativeCallbacks', 'end'], [$callbacks, 'exception']);
echo strrev('original'), "\n";
?>
--EXPECT--
object:before:original
NativeCallbacks:end:degnahc
degnahc
