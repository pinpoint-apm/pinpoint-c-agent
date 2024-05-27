--TEST--
Check  call statck
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

class Foo{
    public  function __construct()
    {
        echo _pinpoint_start_trace()."\n";
        _pinpoint_add_clue("name","Foo");
    }

    public function __destruct()
    {
        echo _pinpoint_end_trace()."\n";
    }

    public function ouput($a,$b)
    {
        _pinpoint_add_clues("1","$a,$b");
        _pinpoint_add_clues("2","234");
    }
}

function ouput_234($a,$b)
{
     $foo = new Foo();
     $foo->ouput("234",3432);
     usleep(2000);
}


echo _pinpoint_start_trace()."\n";

_pinpoint_add_clue("name","ouput_234");
for($x = 0;$x <10;$x++){
    ouput_234(1,3);
}
_pinpoint_add_clue("args",'a,$b');
_pinpoint_add_clue("ret",'a,$b');

echo _pinpoint_end_trace()."\n";


?>
--EXPECTF--
[pinpoint] [%d] [%d] [0] pinpoint_start child  [128]
128
[pinpoint] [%d] [%d] [128] add clue key:name value:ouput_234
[pinpoint] [%d] [%d] [128] pinpoint_start child  [127]
127
[pinpoint] [%d] [%d] [127] add clue key:name value:Foo
[pinpoint] [%d] [%d] [127] add clues:1:234,3432
[pinpoint] [%d] [%d] [127] add clues:2:234
[pinpoint] [%d] [%d] [127] pinpoint_end_trace Done!
128
[pinpoint] [%d] [%d] [128] pinpoint_start child  [126]
126
[pinpoint] [%d] [%d] [126] add clue key:name value:Foo
[pinpoint] [%d] [%d] [126] add clues:1:234,3432
[pinpoint] [%d] [%d] [126] add clues:2:234
[pinpoint] [%d] [%d] [126] pinpoint_end_trace Done!
128
[pinpoint] [%d] [%d] [128] pinpoint_start child  [125]
125
[pinpoint] [%d] [%d] [125] add clue key:name value:Foo
[pinpoint] [%d] [%d] [125] add clues:1:234,3432
[pinpoint] [%d] [%d] [125] add clues:2:234
[pinpoint] [%d] [%d] [125] pinpoint_end_trace Done!
128
[pinpoint] [%d] [%d] [128] pinpoint_start child  [124]
124
[pinpoint] [%d] [%d] [124] add clue key:name value:Foo
[pinpoint] [%d] [%d] [124] add clues:1:234,3432
[pinpoint] [%d] [%d] [124] add clues:2:234
[pinpoint] [%d] [%d] [124] pinpoint_end_trace Done!
128
[pinpoint] [%d] [%d] [128] pinpoint_start child  [123]
123
[pinpoint] [%d] [%d] [123] add clue key:name value:Foo
[pinpoint] [%d] [%d] [123] add clues:1:234,3432
[pinpoint] [%d] [%d] [123] add clues:2:234
[pinpoint] [%d] [%d] [123] pinpoint_end_trace Done!
128
[pinpoint] [%d] [%d] [128] pinpoint_start child  [122]
122
[pinpoint] [%d] [%d] [122] add clue key:name value:Foo
[pinpoint] [%d] [%d] [122] add clues:1:234,3432
[pinpoint] [%d] [%d] [122] add clues:2:234
[pinpoint] [%d] [%d] [122] pinpoint_end_trace Done!
128
[pinpoint] [%d] [%d] [128] pinpoint_start child  [121]
121
[pinpoint] [%d] [%d] [121] add clue key:name value:Foo
[pinpoint] [%d] [%d] [121] add clues:1:234,3432
[pinpoint] [%d] [%d] [121] add clues:2:234
[pinpoint] [%d] [%d] [121] pinpoint_end_trace Done!
128
[pinpoint] [%d] [%d] [128] pinpoint_start child  [120]
120
[pinpoint] [%d] [%d] [120] add clue key:name value:Foo
[pinpoint] [%d] [%d] [120] add clues:1:234,3432
[pinpoint] [%d] [%d] [120] add clues:2:234
[pinpoint] [%d] [%d] [120] pinpoint_end_trace Done!
128
[pinpoint] [%d] [%d] [128] pinpoint_start child  [119]
119
[pinpoint] [%d] [%d] [119] add clue key:name value:Foo
[pinpoint] [%d] [%d] [119] add clues:1:234,3432
[pinpoint] [%d] [%d] [119] add clues:2:234
[pinpoint] [%d] [%d] [119] pinpoint_end_trace Done!
128
[pinpoint] [%d] [%d] [128] pinpoint_start child  [118]
118
[pinpoint] [%d] [%d] [118] add clue key:name value:Foo
[pinpoint] [%d] [%d] [118] add clues:1:234,3432
[pinpoint] [%d] [%d] [118] add clues:2:234
[pinpoint] [%d] [%d] [118] pinpoint_end_trace Done!
128
[pinpoint] [%d] [%d] [128] add clue key:args value:a,$b
[pinpoint] [%d] [%d] [128] add clue key:ret value:a,$b
[pinpoint] [%d] [%d]this span:({":E":%d,":FT":1500,":S":%d,"args":"a,$b","calls":[{":E":%d,":S":%d,"clues":["1:234,3432","2:234"],"name":"Foo"},{":E":%d,":S":%d,"clues":["1:234,3432","2:234"],"name":"Foo"},{":E":%d,":S":%d,"clues":["1:234,3432","2:234"],"name":"Foo"},{":E":%d,":S":%d,"clues":["1:234,3432","2:234"],"name":"Foo"},{":E":%d,":S":%d,"clues":["1:234,3432","2:234"],"name":"Foo"},{":E":%d,":S":%d,"clues":["1:234,3432","2:234"],"name":"Foo"},{":E":%d,":S":%d,"clues":["1:234,3432","2:234"],"name":"Foo"},{":E":%d,":S":%d,"clues":["1:234,3432","2:234"],"name":"Foo"},{":E":%d,":S":%d,"clues":["1:234,3432","2:234"],"name":"Foo"},{":E":%d,":S":%d,"clues":["1:234,3432","2:234"],"name":"Foo"}],"name":"ouput_234","ret":"a,$b"})
[pinpoint] [%d] [%d]agent try to connect:(:/unexist_file.sock)
[pinpoint] [%d] [%d]connect:(:/unexist_file.sock) failed error_code: 2
[pinpoint] [%d] [%d] [128] pinpoint_end_trace Done!
0