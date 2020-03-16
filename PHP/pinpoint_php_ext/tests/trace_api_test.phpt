--TEST--
Check  call statck
--SKIPIF--
<?php if (!extension_loaded("pinpoint_php")) print "skip"; ?>
--INI--
pinpoint_php.CollectorHost=unix:/unexist_file.sock
pinpoint_php.SendSpanTimeOutMs=200
pinpoint_php.UnitTest=true
;pinpoint_php._limit for internal use. User do not use it
pinpoint_php._limit=yes
pinpoint_php.DebugReport=true
--FILE--
<?php 

class Foo{
    public  function __construct()
    {
        echo pinpoint_start_trace()."\n";
        pinpoint_add_clue("name","Foo");
    }

    public function __destruct()
    {
        echo pinpoint_end_trace()."\n";
    }

    public function ouput($a,$b)
    {
        pinpoint_add_clues("1","$a,$b");
        pinpoint_add_clues("2","234");
    }
}

function ouput_234($a,$b)
{
     $foo = new Foo();
     $foo->ouput("234",3432);
     usleep(2000);
}


echo pinpoint_start_trace()."\n";

pinpoint_add_clue("name","ouput_234");
for($x = 0;$x <10;$x++){
    ouput_234(1,3);
}
pinpoint_add_clue("args",'a,$b');
pinpoint_add_clue("ret",'a,$b');

echo pinpoint_end_trace()."\n";


?>
--EXPECTF--
[%d] pinpoint_start_trace 
[%d] agent try to connect:(unix:/unexist_file.sock)
[%d] connect:(/unexist_file.sock) failed as (No such file or directory)
1
[%d] name  -> ouput_234 
[%d] pinpoint_start_trace 
2
[%d] name  -> Foo 
[%d] 1  -> 234,3432 
[%d] 2  -> 234 
[%d] pinpoint_end_trace Done!
1
[%d] pinpoint_start_trace 
2
[%d] name  -> Foo 
[%d] 1  -> 234,3432 
[%d] 2  -> 234 
[%d] pinpoint_end_trace Done!
1
[%d] pinpoint_start_trace 
2
[%d] name  -> Foo 
[%d] 1  -> 234,3432 
[%d] 2  -> 234 
[%d] pinpoint_end_trace Done!
1
[%d] pinpoint_start_trace 
2
[%d] name  -> Foo 
[%d] 1  -> 234,3432 
[%d] 2  -> 234 
[%d] pinpoint_end_trace Done!
1
[%d] pinpoint_start_trace 
2
[%d] name  -> Foo 
[%d] 1  -> 234,3432 
[%d] 2  -> 234 
[%d] pinpoint_end_trace Done!
1
[%d] pinpoint_start_trace 
2
[%d] name  -> Foo 
[%d] 1  -> 234,3432 
[%d] 2  -> 234 
[%d] pinpoint_end_trace Done!
1
[%d] pinpoint_start_trace 
2
[%d] name  -> Foo 
[%d] 1  -> 234,3432 
[%d] 2  -> 234 
[%d] pinpoint_end_trace Done!
1
[%d] pinpoint_start_trace 
2
[%d] name  -> Foo 
[%d] 1  -> 234,3432 
[%d] 2  -> 234 
[%d] pinpoint_end_trace Done!
1
[%d] pinpoint_start_trace 
2
[%d] name  -> Foo 
[%d] 1  -> 234,3432 
[%d] 2  -> 234 
[%d] pinpoint_end_trace Done!
1
[%d] pinpoint_start_trace 
2
[%d] name  -> Foo 
[%d] 1  -> 234,3432 
[%d] 2  -> 234 
[%d] pinpoint_end_trace Done!
1
[%d] args  -> a,$b 
[%d] ret  -> a,$b 
[%d] connect:(/unexist_file.sock) failed as (No such file or directory)
[%d] this span:({"E":%d,"FT":1500,"S":%d,"args":"a,$b","calls":[{"E":%d,"S":%d,"clues":["1:234,3432","2:234"],"name":"Foo"},{"E":%d,"S":%d,"clues":["1:234,3432","2:234"],"name":"Foo"},{"E":%d,"S":%d,"clues":["1:234,3432","2:234"],"name":"Foo"},{"E":%d,"S":%d,"clues":["1:234,3432","2:234"],"name":"Foo"},{"E":%d,"S":%d,"clues":["1:234,3432","2:234"],"name":"Foo"},{"E":%d,"S":%d,"clues":["1:234,3432","2:234"],"name":"Foo"},{"E":%d,"S":%d,"clues":["1:234,3432","2:234"],"name":"Foo"},{"E":%d,"S":%d,"clues":["1:234,3432","2:234"],"name":"Foo"},{"E":%d,"S":%d,"clues":["1:234,3432","2:234"],"name":"Foo"},{"E":%d,"S":%d,"clues":["1:234,3432","2:234"],"name":"Foo"}],"name":"ouput_234","ret":"a,$b"})
[%d] pinpoint_end_trace Done!
0