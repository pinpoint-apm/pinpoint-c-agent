--TEST--
pinpoint_php internal test case [pinpoint_test_main]
--SKIPIF--
<?php if (!extension_loaded("pinpoint_php")) print "skip"; ?>
--INI--
pinpoint_php.CollectorHost=tcp:localhost:10000
pinpoint_php.SendSpanTimeOutMs=-1
pinpoint_php.UnitTest=true
;pinpoint_php._limit for internal use. User do not use it
pinpoint_php._limit=yes
pinpoint_php.DebugReport=true
--FILE--
<?php 

class Foo{
    public  function __construct()
    {
        _pinpoint_start_trace();
        _pinpoint_add_clue("name","Foo");
    }

    public function __destruct()
    {
        _pinpoint_end_trace();
    }

    public function ouput($a,$b)
    {
        _pinpoint_add_clues("1","$a,$b");
        _pinpoint_add_clues("2","234");
    }
}

function Test(){
    throw new \Exception("I am testting");
}

_pinpoint_start_trace();

try{
    $f = new Foo();
    Test();
}catch(\Exception $e){
    throw $e;
}

_pinpoint_start_trace();
--EXPECTF--
[pinpoint] [%d] [%d] [0] pinpoint_start child  [128]
[pinpoint] [%d] [%d] [128] pinpoint_start child  [127]
[pinpoint] [%d] [%d] [127] add clue key:name value:Foo
[pinpoint] [%d] [%d]apm_error_cb called

Fatal error: Uncaught Exception: I am testting in %s:23
Stack trace:
#0 %s(30): Test()
#1 {main}
  thrown in %s on line 23
[pinpoint] [%d] [%d] [127] pinpoint_end_trace Done!
[pinpoint] [%d] [%d]this span:({":E":%d,":FT":1500,":S":%d,"ERR":{"file":"%s","line":23,"msg":"Uncaught Exception: I am testting in %s:23\nStack trace:\n#0 %s(30): Test()\n#1 {main}\n  thrown"},"calls":[{":E":%d,":S":%d,"name":"Foo"}]})
[pinpoint] [%d] [%d][unittest] current span was dropped
[pinpoint] [%d] [%d] [128] pinpoint_end_trace Done!