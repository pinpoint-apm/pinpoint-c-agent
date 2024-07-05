--TEST--
pinpoint_php pinpoint_join_cut_cut
--SKIPIF--
<?php
if (!extension_loaded("pinpoint_php"))
  print "skip";
if (!extension_loaded("curl"))
   print "skip";
if (version_compare(phpversion(), '8.0.0', '<'))
{   
    print "skip";
    print "only works in php8";
}
?>
--INI--
pinpoint_php.DebugReport=true

--EXTENSIONS--
json

--FILE--
<?php
_pinpoint_join_cut(
    ["curl_init"],
    function ($a = null) {
        echo "on_before \n";
        var_dump($a);
    },
    function ($ret) {
        echo "on_end \n";
        var_dump($ret);
        if ($ret instanceof CurlHandle) {
            echo "get resource ";
            assert(curl_setopt($ret, CURLOPT_HEADER, false));
        }
    },
    function ($e) {
        echo "on_exception \n";
        var_dump($e);
    },
);

_pinpoint_join_cut(
    ["curl_exec"],
    function ($a) {
        echo "on_before \n";
        var_dump($a);
        assert($a instanceof CurlHandle);
        echo "request url: " . curl_getinfo($a, CURLINFO_EFFECTIVE_URL) . "\n";
    },
    function ($ret) {
        echo "on_end \n";
    },
    function ($e) {
        echo "on_exception \n";
        var_dump($e);
    },
);

_pinpoint_join_cut(
    ["curl_setopt"],
    function ($ch, $option, $value) {
        assert($ch instanceof CurlHandle);
        var_dump($option);

        if ($option == CURLOPT_HTTPHEADER && is_array($value)) {
            $value[] = "pinpoint_join_cut:xxxx";
            $value[] = "pinpoint_join_cu2t:agc";
            $value[] = "pinpoint_join_cus2t:agc";
            $value[] = "pinpoint_join_cus2t2:agc";
            echo "value must be affected\n";
            return [$ch, $option, $value];
        } else if ($option == CURLOPT_RETURNTRANSFER && is_bool($value)) {
            echo "test CURLOPT_RETURNTRANSFER \n";
            return [$ch, $option, 2];
        } else if ($option == CURLOPT_URL && is_string($value)) {
            echo "test CURLOPT_URL \n";
            return [$ch, 2];
        } else {
            echo "not working";
        }
    },
    function ($ret) {
        echo "on_end \n";
    },
    function ($e) {
        echo "on_exception \n";
        var_dump($e);
    },
);


echo "case: curl_init() \n";

$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, "http://httpbin.org/anything");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_HTTPHEADER, [
    'user_header:xxxx',
    'user_abc:2133'
]);
$response = curl_exec($ch);

$j_res = json_decode($response, true);
assert($j_res["headers"]["Pinpoint-Join-Cu2T"] == "agc");
assert($j_res["headers"]["Pinpoint-Join-Cus2T"] == "agc");
assert($j_res["headers"]["Pinpoint-Join-Cus2T2"] == "agc");
assert($j_res["headers"]["Pinpoint-Join-Cut"] == "xxxx");

$error = curl_error($ch);
echo "error:$error \n";
curl_close($ch);

echo "case : curl_init with variable \n";
$ch = curl_init("http://httpbin.org/anything");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_HTTPHEADER, [
    'user_header:"xxxx"',
    'user_abc:2133'
]);
$response = curl_exec($ch);
$j_res = json_decode($response, true);

assert($j_res["headers"]["Pinpoint-Join-Cu2T"] == "agc");
assert($j_res["headers"]["Pinpoint-Join-Cus2T"] == "agc");
assert($j_res["headers"]["Pinpoint-Join-Cus2T2"] == "agc");
assert($j_res["headers"]["Pinpoint-Join-Cut"] == "xxxx");
$error = curl_error($ch);
echo "error:$error \n";
curl_close($ch);

--EXPECTF--
[pinpoint] [%d] [%d]try to interceptor function=curl_init
[pinpoint] [%d] [%d]added interceptor on `function`: curl_init success
[pinpoint] [%d] [%d]try to interceptor function=curl_exec
[pinpoint] [%d] [%d]added interceptor on `function`: curl_exec success
[pinpoint] [%d] [%d]try to interceptor function=curl_setopt
[pinpoint] [%d] [%d]added interceptor on `function`: curl_setopt success
case: curl_init() 
[pinpoint] [%d] [%d]pinpoint_interceptor_handler_entry: handle func/method:curl_init
on_before 
NULL
[pinpoint] [%d] [%d] call_interceptor_before:curl_init return type(1) zval
[pinpoint] [%d] [%d]replace_ex_caller_parameters return value must be `an array`
on_end 
object(CurlHandle)#10 (0) {
}
get resource [pinpoint] [%d] [%d]pinpoint_interceptor_handler_entry: handle func/method:curl_setopt
int(42)
not working[pinpoint] [%d] [%d] call_interceptor_before:curl_setopt return type(1) zval
[pinpoint] [%d] [%d]replace_ex_caller_parameters return value must be `an array`
on_end 
[pinpoint] [%d] [%d]call_interceptor_end: curl_setopt 
[pinpoint] [%d] [%d]call_interceptor_end: curl_init 
[pinpoint] [%d] [%d]pinpoint_interceptor_handler_entry: handle func/method:curl_setopt
int(10002)
test CURLOPT_URL 
[pinpoint] [%d] [%d] call_interceptor_before:curl_setopt return type(7) zval
[pinpoint] [%d] [%d]argv size:2
[pinpoint] [%d] [%d]error: replace_ex_caller_parameters return `size` does not matched
on_end 
[pinpoint] [%d] [%d]call_interceptor_end: curl_setopt 
[pinpoint] [%d] [%d]pinpoint_interceptor_handler_entry: handle func/method:curl_setopt
int(19913)
test CURLOPT_RETURNTRANSFER 
[pinpoint] [%d] [%d] call_interceptor_before:curl_setopt return type(7) zval
[pinpoint] [%d] [%d]argv size:3
[pinpoint] [%d] [%d]error: replace_ex_caller_parameters return `type` does not matched expected:3 give:4
on_end 
[pinpoint] [%d] [%d]call_interceptor_end: curl_setopt 
[pinpoint] [%d] [%d]pinpoint_interceptor_handler_entry: handle func/method:curl_setopt
int(10023)
value must be affected
[pinpoint] [%d] [%d] call_interceptor_before:curl_setopt return type(7) zval
[pinpoint] [%d] [%d]argv size:3
[pinpoint] [%d] [%d]args[2] type:7 in:1 va:2
on_end 
[pinpoint] [%d] [%d]call_interceptor_end: curl_setopt 
[pinpoint] [%d] [%d]pinpoint_interceptor_handler_entry: handle func/method:curl_exec
on_before 
object(CurlHandle)#10 (0) {
}
request url: http://httpbin.org/anything
[pinpoint] [%d] [%d] call_interceptor_before:curl_exec return type(1) zval
[pinpoint] [%d] [%d]replace_ex_caller_parameters return value must be `an array`
on_end 
[pinpoint] [%d] [%d]call_interceptor_end: curl_exec 
error: 
case : curl_init with variable 
[pinpoint] [%d] [%d]pinpoint_interceptor_handler_entry: handle func/method:curl_init
on_before 
string(27) "http://httpbin.org/anything"
[pinpoint] [%d] [%d] call_interceptor_before:curl_init return type(1) zval
[pinpoint] [%d] [%d]replace_ex_caller_parameters return value must be `an array`
on_end 
object(CurlHandle)#11 (0) {
}
get resource [pinpoint] [%d] [%d]pinpoint_interceptor_handler_entry: handle func/method:curl_setopt
int(42)
not working[pinpoint] [%d] [%d] call_interceptor_before:curl_setopt return type(1) zval
[pinpoint] [%d] [%d]replace_ex_caller_parameters return value must be `an array`
on_end 
[pinpoint] [%d] [%d]call_interceptor_end: curl_setopt 
[pinpoint] [%d] [%d]call_interceptor_end: curl_init 
[pinpoint] [%d] [%d]pinpoint_interceptor_handler_entry: handle func/method:curl_setopt
int(19913)
test CURLOPT_RETURNTRANSFER 
[pinpoint] [%d] [%d] call_interceptor_before:curl_setopt return type(7) zval
[pinpoint] [%d] [%d]argv size:3
[pinpoint] [%d] [%d]error: replace_ex_caller_parameters return `type` does not matched expected:3 give:4
on_end 
[pinpoint] [%d] [%d]call_interceptor_end: curl_setopt 
[pinpoint] [%d] [%d]pinpoint_interceptor_handler_entry: handle func/method:curl_setopt
int(10023)
value must be affected
[pinpoint] [%d] [%d] call_interceptor_before:curl_setopt return type(7) zval
[pinpoint] [%d] [%d]argv size:3
[pinpoint] [%d] [%d]args[2] type:7 in:1 va:2
on_end 
[pinpoint] [%d] [%d]call_interceptor_end: curl_setopt 
[pinpoint] [%d] [%d]pinpoint_interceptor_handler_entry: handle func/method:curl_exec
on_before 
object(CurlHandle)#11 (0) {
}
request url: http://httpbin.org/anything
[pinpoint] [%d] [%d] call_interceptor_before:curl_exec return type(1) zval
[pinpoint] [%d] [%d]replace_ex_caller_parameters return value must be `an array`
on_end 
[pinpoint] [%d] [%d]call_interceptor_end: curl_exec 
error: 
[pinpoint] [%d] [%d]start free interceptor: curl_init
[pinpoint] [%d] [%d]start free interceptor: curl_exec
[pinpoint] [%d] [%d]start free interceptor: curl_setopt