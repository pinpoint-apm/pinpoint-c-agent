<?php


namespace example\swoole\Plugins;
use example\swoole\Plugins\Candy;

///@hook:example\UserManagerment::checkUser example\UserManagerment::register example\UserManagerment::cacheUser
class CommonPlugin extends Candy
{
    public function onBefore(){

        pinpoint_add_clue("stp",PHP_METHOD);
        pinpoint_add_clues(PHP_ARGS,print_r($this->args,true));
    }

    public function onEnd(&$ret){
        var_dump(print_r($ret,true));
        pinpoint_add_clues(PHP_RETURN, print_r($ret,true));
    }

    public function onException($e){
        pinpoint_add_clue("EXP",$e->getMessage());
    }
}