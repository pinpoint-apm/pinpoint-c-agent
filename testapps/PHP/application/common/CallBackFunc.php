<?php


namespace app\common;


class CallBackFunc
{
    static function fnCallback1($msg1, $msg2){
        return "msg1:".$msg1."<br/>"."msg2:".$msg2."<br/>";
    }

    public function fnCallback2(){
        return "public function.";
    }
}