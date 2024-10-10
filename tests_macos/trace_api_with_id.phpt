--TEST--
Test pinpoint_unique_id
--SKIPIF--
<?php if (!extension_loaded("pinpoint_php")) print "skip"; ?>
--INI--
pinpoint_php.CollectorHost=tcp:localhost:10000
pinpoint_php.SendSpanTimeOutMs=-1
pinpoint_php.UnitTest=true
pinpoint_php.DebugReport=true
--FILE--
<?php
$id = 0;

function exception_handler($exception) {
    global $id;

    $lastId = $id;
    _pinpoint_add_clue("EXP",$exception->getMessage(),$id);

    do
    {
        $lastId = _pinpoint_end_trace($lastId);
    }while($lastId != 0);
    echo "Uncaught exception: " , $exception->getMessage(), "\n";
}

set_exception_handler('exception_handler');

$id = _pinpoint_start_trace($id);
_pinpoint_add_clue("name","Foo",$id);
$id = _pinpoint_start_trace($id);
_pinpoint_add_clues("name","Foo1",$id);
$id = _pinpoint_start_trace($id);
_pinpoint_add_clue("name","Foo2",$id);
$id = _pinpoint_end_trace($id);
$id = _pinpoint_start_trace($id);
_pinpoint_add_clues("name","Foo3",$id);
_pinpoint_add_clue("name","Foo4",$id);
$id = _pinpoint_end_trace($id);
_pinpoint_add_clues("name","Foo5",$id);
throw  new Exception("too heavy, throw it");
$id = _pinpoint_end_trace($id);
_pinpoint_add_clue("name","Foo",$id);

$id = _pinpoint_end_trace($id);

--EXPECTF--
[pinpoint] [%d:%d] [0] pinpoint_start child [128]
[pinpoint] [%d:%d] [128] add anno_v1 key:name value:Foo
[pinpoint] [%d:%d] [128] pinpoint_start child [127]
[pinpoint] [%d:%d] [127] add anno_v2 name:Foo1
[pinpoint] [%d:%d] [127] pinpoint_start child [126]
[pinpoint] [%d:%d] [126] add anno_v1 key:name value:Foo2
[pinpoint] [%d:%d] [126] pinpoint_end_trace Done!
[pinpoint] [%d:%d] [127] pinpoint_start child [125]
[pinpoint] [%d:%d] [125] add anno_v2 name:Foo3
[pinpoint] [%d:%d] [125] add anno_v1 key:name value:Foo4
[pinpoint] [%d:%d] [125] pinpoint_end_trace Done!
[pinpoint] [%d:%d] [127] add anno_v2 name:Foo5
[pinpoint] [%d:%d] [127] add anno_v1 key:EXP value:too heavy, throw it
[pinpoint] [%d:%d] [127] pinpoint_end_trace Done!
[pinpoint] [%d:%d]this span:({":E":%d,":FT":1500,":S":%d,"event":[{":E":%d,":S":%d,":depth":1,":seq":0,"EXP":"too heavy, throw it","anno":["name:Foo1","name:Foo5"]},{":E":%d,":S":%d,":depth":2,":seq":1,"name":"Foo2"},{":E":%d,":S":%d,":depth":2,":seq":2,"anno":["name:Foo3"],"name":"Foo4"}],"name":"Foo"})
[pinpoint] [%d:%d][unittest] current span was dropped
[pinpoint] [%d:%d] [128] pinpoint_end_trace Done!
Uncaught exception: too heavy, throw it