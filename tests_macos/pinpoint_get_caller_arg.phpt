--TEST--
pinpoint_php test  pinpoint_get_caller_arg
--SKIPIF--
<?php 
if (!extension_loaded("pinpoint_php")) 
{   
    print "skip"; 
    print "pinpoint_php"; 
}
if (!extension_loaded("curl")) 
{   
    print "skip"; 
    print "curl"; 
}

--INI--
pinpoint_php.DebugReport=true
--EXTENSIONS--
curl
--FILE--
<?php

_pinpoint_join_cut(
    ["curl_exec"],
    function ($a) {
    },
    function ($ret) {
        $ch = pinpoint_get_caller_arg(0);
        $code = curl_getinfo($ch, CURLINFO_HTTP_CODE);
        echo "status_code $code \n";
    },
    function ($e) {
    }
);

echo "case: curl_init() \n";

$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, "http://httpbin.org/anything");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_HTTPHEADER, [
    'user_header:"xxxx"',
    'user_abc:2133'
]);
$response = curl_exec($ch);

$j_res = json_decode($response, true);

$error = curl_error($ch);
echo "error: $error\n";
curl_close($ch);

--EXPECTF--
[pinpoint] [%d] [%d]try to interceptor function=curl_exec
[pinpoint] [%d] [%d]added interceptor on `function`: curl_exec success
case: curl_init() 
[pinpoint] [%d] [%d]pinpoint_interceptor_handler_entry: handle func/method:curl_exec
[pinpoint] [%d] [%d] call_interceptor_before:curl_exec return type(1) zval
[pinpoint] [%d] [%d]replace_ex_caller_parameters return value must be `an array`
status_code 200 
[pinpoint] [%d] [%d]call_interceptor_end: curl_exec 
error: 
[pinpoint] [%d] [%d]start free interceptor: curl_exec