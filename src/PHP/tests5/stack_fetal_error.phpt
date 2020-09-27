--TEST--
pinpioint_php internal test case [pinpoint_test_main]
--SKIPIF--
<?php if (!extension_loaded("pinpoint_php")) print "skip"; ?>
--INI--
pinpoint_php.CollectorHost=unix:/unexist_file.sock
pinpoint_php.SendSpanTimeOutMs=0
pinpoint_php.UnitTest=true
;pinpoint_php._limit for internal use. User do not use it
pinpoint_php._limit=yes
pinpoint_php.DebugReport=true
--FILE--
<?php 
//function shutdown()
//{
//    while(pinpoint_end_trace()>0);
//}
//register_shutdown_function('shutdown');
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
[pinpoint] [%d] [%d]#0 pinpoint_start start
[pinpoint] [%d] [%d]#128 pinpoint_start start
[pinpoint] [%d] [%d]#127 add clue key:name value:Foo
[pinpoint] [%d] [%d]apm_error_cb called

Fatal error: Uncaught exception 'Exception' with message 'I am testting' in %s:27
Stack trace:
#0 %s(34): Test()
#1 {main}
  thrown in %s on line 27
[pinpoint] [%d] [%d]this span:({"E":0,"ERR":{"file":"%s","line":27,"msg":"Uncaught exception 'Exception' with message 'I am testting' in %s:27\nStack trace:\n#0 %s(34): Test()\n#1 {main}\n  thrown"},"FT":1500,"S":%d,"calls":[{"S":0,"name":"Foo"}]})
[pinpoint] [%d] [%d]agent try to connect:(unix:/unexist_file.sock)
[pinpoint] [%d] [%d]connect:(/unexist_file.sock) failed as (No such file or directory)
[pinpoint] [%d] [%d]#0 pinpoint_end_trace Done!