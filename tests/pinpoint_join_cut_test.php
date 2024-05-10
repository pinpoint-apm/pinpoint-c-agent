<?php

pinpoint_join_cut(
    ["curl_init"],
    function ($a = null) {
        echo "on_before \n";
        var_dump($a);
    },
    function ($ret) {
        echo "on_end \n";
        var_dump($ret);
        if (is_resource($ret)) {
            echo "get resource ";
            assert(curl_setopt($ret, CURLOPT_HEADER, false));
        }
    },
    function ($e) {
        echo "on_exception \n";
        var_dump($e);
    },
);


pinpoint_join_cut(
    ["curl_exec"],
    function ($a) {
        echo "on_before \n";
        var_dump($a);
        assert(is_resource($a));
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

pinpoint_join_cut(
    ["curl_setopt"],
    function ($ch, $option, $value) {
        assert(is_resource($ch));
        var_dump($option);
        var_dump($value);

        if ($option == CURLOPT_HTTPHEADER && is_array($value)) {
            $value[] = "pinpoint_join_cut:xxxx";
            $value[] = "pinpoint_join_cu2t:agc";
            $value[] = "pinpoint_join_cus2t:agc";
            $value[] = "pinpoint_join_cus2t:agc";
            echo "value must be affected\n";
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
curl_setopt($ch, CURLOPT_URL, "http://httpbin/anything");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_HTTPHEADER, [
    'user_header:"xxxx"',
    'user_abc:2133'
]);
$response = curl_exec($ch);
echo "\n response= $response \n";
$error = curl_error($ch);
echo "error:$error \n";
curl_close($ch);

echo "case : curl_init with variable \n";
$ch = curl_init("http://httpbin/anything");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_HTTPHEADER, [
    'user_header:"xxxx"',
    'user_abc:2133'
]);
$response = curl_exec($ch);
echo "\n response= $response \n";
$error = curl_error($ch);
echo "error:$error \n";
curl_close($ch);

