<?php
namespace Plugins;
use Plugins\Candy;

///@hook:app\Foo::foo_p3_rbool app\Foo::foo_p3_rfloat app\Foo::foo_p3 app\Foo::foo_p1
/// @hook:test wrong format
///@hook:app\AppDate::\date app1\Foo::test_func_2 app\AopFunction::test_func1 app\TestError::Strict
///@hook:app\AppDate::outputDate
class CommonPlugin extends Candy
{
    ///@hook:app\DBcontrol::connectDb
    public function onBefore(){
        pinpoint_add_clue("stp",PHP_METHOD);
        pinpoint_add_clues(PHP_ARGS,print_r($this->args,true));
    }

    ///@hook:app\DBcontrol::getData1 app\DBcontrol::\array_push
    public function onEnd(&$ret){
        var_dump($ret);
        pinpoint_add_clues(PHP_RETURN,"ture");
    }

    ///@hook:app\DBcontrol::getData2
    public function onException($e){
        pinpoint_add_clue("EXP",$e->getMessage());
    }
}
