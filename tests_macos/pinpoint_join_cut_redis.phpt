--TEST--
pinpoint_php pinpoint_join_cut_redis
--SKIPIF--
<?php
if (!extension_loaded("pinpoint_php"))
  print "skip";

if (!extension_loaded("redis"))
  print "skip";

if (version_compare(phpversion(), '7.1.33', '<='))
   print "skip";

if(getenv('SKIP_MACOS_ACTION', true)){
  print "skip";
}
?>
--INI--
pinpoint_php.DebugReport=true
--EXTENSIONS--
json
redis
--FILE--
<?php
function make_method_plugins_var_2($class_name, $method_name)
{
    //  connect 
    $interceptor_name = "$class_name::$method_name";
    $on_before_2 = function ($key, $value) use ($interceptor_name) {
        echo "$interceptor_name before: $key : $value \n";
    };

    $on_end = function ($ret) use ($interceptor_name) {
        echo "$interceptor_name end \n";
    };

    $on_exception = function ($exp) use ($interceptor_name) {
        echo "$interceptor_name exception \n";
    };

    return [$class_name, $method_name, $on_before_2, $on_end, $on_exception];
}

function make_method_plugins_var_1($class_name, $method_name)
{
    $interceptor_name = "$class_name::$method_name";

    $on_before_1 = function ($key = null) use ($interceptor_name) {
        echo "$interceptor_name before: $key \n";
    };

    $on_end = function ($ret) use ($interceptor_name) {
        echo "$interceptor_name end \n";
    };

    $on_exception = function ($exp) use ($interceptor_name) {
        echo "$interceptor_name exception \n";
    };

    return [$class_name, $method_name, $on_before_1, $on_end, $on_exception];
}

function make_method_plugins_var_3($class_name, $method_name)
{
    $interceptor_name = "$class_name::$method_name";

    $on_before_1 = function ($key, $_1, $_2) use ($interceptor_name) {
        echo "$interceptor_name before: $key \n";
    };

    $on_end = function ($ret) use ($interceptor_name) {
        echo "$interceptor_name end \n";
    };

    $on_exception = function ($exp) use ($interceptor_name) {
        echo "$interceptor_name exception \n";
    };

    return [$class_name, $method_name, $on_before_1, $on_end, $on_exception];
}


function make_method_plugins_var_ar($class_name, $method_name)
{
    $interceptor_name = "$class_name::$method_name";

    $on_before_1 = function ($ar) use ($interceptor_name) {
        echo "$interceptor_name before:  \n";
    };

    $on_end = function ($ret) use ($interceptor_name) {
        echo "$interceptor_name end \n";
    };

    $on_exception = function ($exp) use ($interceptor_name) {
        echo "$interceptor_name exception \n";
    };

    return [$class_name, $method_name, $on_before_1, $on_end, $on_exception];
}

$points = [
    make_method_plugins_var_1("Redis", "info"),
    make_method_plugins_var_1("Redis", "get"),
    make_method_plugins_var_2("Redis", "set"),
    make_method_plugins_var_2("Redis", "setNx"),
    make_method_plugins_var_2("Redis", "append"),
    make_method_plugins_var_3("Redis", "getRange"),
    make_method_plugins_var_3("Redis", "setRange"),
    make_method_plugins_var_1("Redis", "strlen"),
    make_method_plugins_var_2("Redis", "getBit"),
    make_method_plugins_var_3("Redis", "setBit"),
    make_method_plugins_var_ar("Redis", "mSet"),
    make_method_plugins_var_ar("Redis", "mSetNx"),
];

foreach ($points as $point) {
    _pinpoint_join_cut(
        [$point[0], $point[1]],
        $point[2],
        $point[3],
        $point[4]
    );
}


echo "case: redis(xxx) \n";

$redis = new Redis([
    'host' => 'redis',
    'port' => 6379,
    'connectTimeout' => 2.5
]);
assert($redis->info() !== false);

echo "case: redis() \n";
$redis = new Redis();
$redis->connect('redis', 6379);
assert($redis->info() !== false);

echo "case: redis() \n";
$redis = new Redis();
$redis->pconnect('redis', 6379);
assert($redis->info() !== false);

echo "case: strings \n";
$redis = new Redis();
$redis->pconnect('redis', 6379);
$redis->get("a");
$redis->set("a", "b");
$redis->setNx("a", "10");
$redis->append("ar", "aa");
$redis->setRange("ar", 0, "bb");
$redis->get("ar");
$redis->getRange("ar", 0, 2);
$redis->strlen("ar");
$redis->getBit("ar", 1);
$redis->setBit("ar", 1, 3);
var_dump($redis->get("ar"));
$redis->mSet(['foo' => 'foo', 'bar' => 'bar', 'baz' => 'baz']);
var_dump($redis->exists('foo', 'bar', 'baz', 'not exists')); 
$redis->flushdb();
--EXPECTF--
[pinpoint] [%d] [%d]try to interceptor module(class)/function=redis:info
[pinpoint] [%d] [%d]added interceptor on `module`: redis::info success
[pinpoint] [%d] [%d]try to interceptor module(class)/function=redis:get
[pinpoint] [%d] [%d]added interceptor on `module`: redis::get success
[pinpoint] [%d] [%d]try to interceptor module(class)/function=redis:set
[pinpoint] [%d] [%d]added interceptor on `module`: redis::set success
[pinpoint] [%d] [%d]try to interceptor module(class)/function=redis:setnx
[pinpoint] [%d] [%d]added interceptor on `module`: redis::setnx success
[pinpoint] [%d] [%d]try to interceptor module(class)/function=redis:append
[pinpoint] [%d] [%d]added interceptor on `module`: redis::append success
[pinpoint] [%d] [%d]try to interceptor module(class)/function=redis:getrange
[pinpoint] [%d] [%d]added interceptor on `module`: redis::getrange success
[pinpoint] [%d] [%d]try to interceptor module(class)/function=redis:setrange
[pinpoint] [%d] [%d]added interceptor on `module`: redis::setrange success
[pinpoint] [%d] [%d]try to interceptor module(class)/function=redis:strlen
[pinpoint] [%d] [%d]added interceptor on `module`: redis::strlen success
[pinpoint] [%d] [%d]try to interceptor module(class)/function=redis:getbit
[pinpoint] [%d] [%d]added interceptor on `module`: redis::getbit success
[pinpoint] [%d] [%d]try to interceptor module(class)/function=redis:setbit
[pinpoint] [%d] [%d]added interceptor on `module`: redis::setbit success
[pinpoint] [%d] [%d]try to interceptor module(class)/function=redis:mset
[pinpoint] [%d] [%d]added interceptor on `module`: redis::mset success
[pinpoint] [%d] [%d]try to interceptor module(class)/function=redis:msetnx
[pinpoint] [%d] [%d]added interceptor on `module`: redis::msetnx success
case: redis(xxx) 
[pinpoint] [%d] [%d]pinpoint_interceptor_handler_entry: handle func/method:redis::info
Redis::info before:  
[pinpoint] [%d] [%d] call_interceptor_before:redis::info return type(1) zval
[pinpoint] [%d] [%d]replace_ex_caller_parameters return value must be `an array`
Redis::info end 
[pinpoint] [%d] [%d]call_interceptor_end: redis::info 
case: redis() 
[pinpoint] [%d] [%d]pinpoint_interceptor_handler_entry: handle func/method:redis::info
Redis::info before:  
[pinpoint] [%d] [%d] call_interceptor_before:redis::info return type(1) zval
[pinpoint] [%d] [%d]replace_ex_caller_parameters return value must be `an array`
Redis::info end 
[pinpoint] [%d] [%d]call_interceptor_end: redis::info 
case: redis() 
[pinpoint] [%d] [%d]pinpoint_interceptor_handler_entry: handle func/method:redis::info
Redis::info before:  
[pinpoint] [%d] [%d] call_interceptor_before:redis::info return type(1) zval
[pinpoint] [%d] [%d]replace_ex_caller_parameters return value must be `an array`
Redis::info end 
[pinpoint] [%d] [%d]call_interceptor_end: redis::info 
case: strings 
[pinpoint] [%d] [%d]pinpoint_interceptor_handler_entry: handle func/method:redis::get
Redis::get before: a 
[pinpoint] [%d] [%d] call_interceptor_before:redis::get return type(1) zval
[pinpoint] [%d] [%d]replace_ex_caller_parameters return value must be `an array`
Redis::get end 
[pinpoint] [%d] [%d]call_interceptor_end: redis::get 
[pinpoint] [%d] [%d]pinpoint_interceptor_handler_entry: handle func/method:redis::set
Redis::set before: a : b 
[pinpoint] [%d] [%d] call_interceptor_before:redis::set return type(1) zval
[pinpoint] [%d] [%d]replace_ex_caller_parameters return value must be `an array`
Redis::set end 
[pinpoint] [%d] [%d]call_interceptor_end: redis::set 
[pinpoint] [%d] [%d]pinpoint_interceptor_handler_entry: handle func/method:redis::setnx
Redis::setNx before: a : 10 
[pinpoint] [%d] [%d] call_interceptor_before:redis::setnx return type(1) zval
[pinpoint] [%d] [%d]replace_ex_caller_parameters return value must be `an array`
Redis::setNx end 
[pinpoint] [%d] [%d]call_interceptor_end: redis::setnx 
[pinpoint] [%d] [%d]pinpoint_interceptor_handler_entry: handle func/method:redis::append
Redis::append before: ar : aa 
[pinpoint] [%d] [%d] call_interceptor_before:redis::append return type(1) zval
[pinpoint] [%d] [%d]replace_ex_caller_parameters return value must be `an array`
Redis::append end 
[pinpoint] [%d] [%d]call_interceptor_end: redis::append 
[pinpoint] [%d] [%d]pinpoint_interceptor_handler_entry: handle func/method:redis::setrange
Redis::setRange before: ar 
[pinpoint] [%d] [%d] call_interceptor_before:redis::setrange return type(1) zval
[pinpoint] [%d] [%d]replace_ex_caller_parameters return value must be `an array`
Redis::setRange end 
[pinpoint] [%d] [%d]call_interceptor_end: redis::setrange 
[pinpoint] [%d] [%d]pinpoint_interceptor_handler_entry: handle func/method:redis::get
Redis::get before: ar 
[pinpoint] [%d] [%d] call_interceptor_before:redis::get return type(1) zval
[pinpoint] [%d] [%d]replace_ex_caller_parameters return value must be `an array`
Redis::get end 
[pinpoint] [%d] [%d]call_interceptor_end: redis::get 
[pinpoint] [%d] [%d]pinpoint_interceptor_handler_entry: handle func/method:redis::getrange
Redis::getRange before: ar 
[pinpoint] [%d] [%d] call_interceptor_before:redis::getrange return type(1) zval
[pinpoint] [%d] [%d]replace_ex_caller_parameters return value must be `an array`
Redis::getRange end 
[pinpoint] [%d] [%d]call_interceptor_end: redis::getrange 
[pinpoint] [%d] [%d]pinpoint_interceptor_handler_entry: handle func/method:redis::strlen
Redis::strlen before: ar 
[pinpoint] [%d] [%d] call_interceptor_before:redis::strlen return type(1) zval
[pinpoint] [%d] [%d]replace_ex_caller_parameters return value must be `an array`
Redis::strlen end 
[pinpoint] [%d] [%d]call_interceptor_end: redis::strlen 
[pinpoint] [%d] [%d]pinpoint_interceptor_handler_entry: handle func/method:redis::getbit
Redis::getBit before: ar : 1 
[pinpoint] [%d] [%d] call_interceptor_before:redis::getbit return type(1) zval
[pinpoint] [%d] [%d]replace_ex_caller_parameters return value must be `an array`
Redis::getBit end 
[pinpoint] [%d] [%d]call_interceptor_end: redis::getbit 
[pinpoint] [%d] [%d]pinpoint_interceptor_handler_entry: handle func/method:redis::setbit
Redis::setBit before: ar 
[pinpoint] [%d] [%d] call_interceptor_before:redis::setbit return type(1) zval
[pinpoint] [%d] [%d]replace_ex_caller_parameters return value must be `an array`
Redis::setBit end 
[pinpoint] [%d] [%d]call_interceptor_end: redis::setbit 
[pinpoint] [%d] [%d]pinpoint_interceptor_handler_entry: handle func/method:redis::get
Redis::get before: ar 
[pinpoint] [%d] [%d] call_interceptor_before:redis::get return type(1) zval
[pinpoint] [%d] [%d]replace_ex_caller_parameters return value must be `an array`
Redis::get end 
[pinpoint] [%d] [%d]call_interceptor_end: redis::get 
string(2) "bb"
[pinpoint] [%d] [%d]pinpoint_interceptor_handler_entry: handle func/method:redis::mset
Redis::mSet before:  
[pinpoint] [%d] [%d] call_interceptor_before:redis::mset return type(1) zval
[pinpoint] [%d] [%d]replace_ex_caller_parameters return value must be `an array`
Redis::mSet end 
[pinpoint] [%d] [%d]call_interceptor_end: redis::mset 
int(3)
[pinpoint] [%d] [%d]start free interceptor: redis::info
[pinpoint] [%d] [%d]start free interceptor: redis::get
[pinpoint] [%d] [%d]start free interceptor: redis::set
[pinpoint] [%d] [%d]start free interceptor: redis::setnx
[pinpoint] [%d] [%d]start free interceptor: redis::append
[pinpoint] [%d] [%d]start free interceptor: redis::getrange
[pinpoint] [%d] [%d]start free interceptor: redis::setrange
[pinpoint] [%d] [%d]start free interceptor: redis::strlen
[pinpoint] [%d] [%d]start free interceptor: redis::getbit
[pinpoint] [%d] [%d]start free interceptor: redis::setbit
[pinpoint] [%d] [%d]start free interceptor: redis::mset
[pinpoint] [%d] [%d]start free interceptor: redis::msetnx