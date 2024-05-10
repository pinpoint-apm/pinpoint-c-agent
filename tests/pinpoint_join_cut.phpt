--TEST--
pinpoint_php test get this
--SKIPIF--
<?php
if (!extension_loaded("pinpoint_php"))
  print "skip";
if (!extension_loaded("curl"))
  print "skip";
if (!extension_loaded("pdo"))
  print "skip";
?>
--INI--

--FILE--
<?php 
$ch = curl_init("http://www.example.com/");
$fp = fopen("example_homepage.txt", "w");

curl_setopt($ch, CURLOPT_FILE, $fp);
curl_setopt($ch, CURLOPT_HEADER, 0);

curl_exec($ch);
if(curl_error($ch)) {
fwrite($fp, curl_error($ch));
}
curl_close($ch);
fclose($fp);

--EXPECTF--
xvd