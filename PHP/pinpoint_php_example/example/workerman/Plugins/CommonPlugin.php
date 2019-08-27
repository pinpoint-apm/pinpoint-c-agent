<?php


namespace example\workerman\Plugins;
use example\workerman\Plugins\Candy;

///@hook:example\UserManagerment::checkUser example\UserManagerment::register example\UserManagerment::cacheUser
class CommonPlugin extends Candy
{
    public function onBefore(){

        pinpoint_add_clue("stp",PHP_METHOD);
        pinpoint_add_clues(PHP_ARGS,print_r($this->args,true));
    }

    public function onEnd(&$ret){
        pinpoint_add_clues(PHP_RETURN,'xxx');
    }

    public function onException($e){
        pinpoint_add_clue("EXP",$e->getMessage());
    }
}