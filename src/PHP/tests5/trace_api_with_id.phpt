--TEST--
Test pinpoint_unique_id
--SKIPIF--
<?php if (!extension_loaded("pinpoint_php")) print "skip"; ?>
--INI--
pinpoint_php.CollectorHost=unix:/unexist_file.sock
pinpoint_php.SendSpanTimeOutMs=0
pinpoint_php.UnitTest=true
pinpoint_php.DebugReport=true
--FILE--
<?php
$id = 0;

function exception_handler($exception) {
    global $id;

    $lastId = $id;
    pinpoint_add_clue("EXP",$exception->getMessage(),$id);

    do
    {
        $lastId = pinpoint_end_trace($lastId);
    }while($lastId != 0);
    echo "Uncaught exception: " , $exception->getMessage(), "\n";
}

set_exception_handler('exception_handler');

$id = pinpoint_start_trace($id);
pinpoint_add_clue("name","Foo",$id);
$id = pinpoint_start_trace($id);
pinpoint_add_clues("name","Foo1",$id);
$id = pinpoint_start_trace($id);
pinpoint_add_clue("name","Foo2",$id);
$id = pinpoint_end_trace($id);
$id = pinpoint_start_trace($id);
pinpoint_add_clues("name","Foo3",$id);
pinpoint_add_clue("name","Foo4",$id);
$id = pinpoint_end_trace($id);
pinpoint_add_clues("name","Foo5",$id);
throw  new Exception("too heavy, throw it");
$id = pinpoint_end_trace($id);
pinpoint_add_clue("name","Foo",$id);

$id = pinpoint_end_trace($id);

--EXPECTF--
[pinpoint] [%d] [%d]#0 pinpoint_start start
[pinpoint] [%d] [%d]#128 add clue key:name value:Foo
[pinpoint] [%d] [%d]#128 pinpoint_start start
[pinpoint] [%d] [%d]#127 add clues:name:Foo1
[pinpoint] [%d] [%d]#127 pinpoint_start start
[pinpoint] [%d] [%d]#126 add clue key:name value:Foo2
[pinpoint] [%d] [%d]#126 pinpoint_end_trace Done!
[pinpoint] [%d] [%d]#127 pinpoint_start start
[pinpoint] [%d] [%d]#125 add clues:name:Foo3
[pinpoint] [%d] [%d]#125 add clue key:name value:Foo4
[pinpoint] [%d] [%d]#125 pinpoint_end_trace Done!
[pinpoint] [%d] [%d]#127 add clues:name:Foo5
[pinpoint] [%d] [%d]#127 add clue key:EXP value:too heavy, throw it
[pinpoint] [%d] [%d]#127 pinpoint_end_trace Done!
[pinpoint] [%d] [%d]this span:({"E":%d,"FT":1500,"S":%d,"calls":[{"E":%d,"EXP":"too heavy, throw it","S":%d,"calls":[{"E":%d,"S":%d,"name":"Foo2"},{"E":%d,"S":%d,"clues":["name:Foo3"],"name":"Foo4"}],"clues":["name:Foo1","name:Foo5"]}],"name":"Foo"})
[pinpoint] [%d] [%d]agent try to connect:(unix:/unexist_file.sock)
[pinpoint] [%d] [%d]connect:(/unexist_file.sock) failed as (No such file or directory)
[pinpoint] [%d] [%d]#128 pinpoint_end_trace Done!
Uncaught exception: too heavy, throw it