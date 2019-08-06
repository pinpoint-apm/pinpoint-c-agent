--TEST--
pinpioint_php internal test case [pinpoint_test_main]
--SKIPIF--
<?php if (!extension_loaded("pinpoint_php")) print "skip"; ?>
--INI--
pinpoint_php.CollectorHost=unix:/unexist_file.sock
pinpoint_php.SendSpanTimeOutMs=0
pinpoint_php.UnitTest=true
--FILE--
<?php 
function shutdown()
{
    while(pinpoint_end_trace()>0);
}
register_shutdown_function('shutdown');
class Foo{
    public  function __construct()
    {
        pinpoint_start_trace();
        pinpoint_add_clue("name","Foo");
    }

    public function __destruct()
    {
        pinpoint_end_trace();
    }

    public function ouput($a,$b)
    {
        pinpoint_add_clues("1","$a,$b");
        pinpoint_add_clues("2","234");
    }
}

function Test(){
    throw new \Exception("I am testting");
}

pinpoint_start_trace();

try{
    $f = new Foo();
    Test();
}catch(\Exception $e){
    throw $e;
}

pinpoint_start_trace();
--EXPECTF--
[%d] pinpoint_start_trace 
[%d] agent try to connect:(unix:/unexist_file.sock)
[%d] connect:(/unexist_file.sock) failed as (No such file or directory)
[%d] pinpoint_start_trace 
[%d] name  -> Foo 
[%d] apm_error_cb called

Fatal error: Uncaught Exception: I am testting in %s:%d
Stack trace:
#0 %s(34): Test()
#1 {main}
  thrown in %s on line %d
[%d] pinpoint_end_trace Done!
[%d] connect:(/unexist_file.sock) failed as (No such file or directory)
[%d] this span:({"E":%d,"ERR":{"file":"%s","line":%d,"msg":"Uncaught Exception: I am testting in %s:%d\nStack trace:\n#0 %s(34): Test()\n#1 {main}\n  thrown"},"S":%d,"calls":[{"E":%d,"S":%d,"name":"Foo"}]})
[%d] pinpoint_end_trace Done!