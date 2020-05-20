<?php


namespace app;


class CallBackFunc
{
    static function fnCallback1($msg1, $msg2){
        echo "msg1:".$msg1."<br/>";
        echo "msg2:".$msg2."<br/>";
    }

    public function fnCallback2(){
        echo "public function.";
    }
}