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

pinpoint_join_cut(
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

pinpoint_join_cut(
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
