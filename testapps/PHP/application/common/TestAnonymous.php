<?php


namespace app\common;
use app\common\AopFunction;

class TestAnonymous
{
    public function createbell($time) {
        return function() use ($time) {

            $res1 = "$time". "di di di  di ...";
            $func1 = new AopFunction();
//            try{
//                throw new \Exception("exception in the closure.");
//            }catch (\Exception $e){
//                echo $e->getMessage();
//            }
            $res2 = $func1->test_func1("x", 3);
            return $res1.$res2;
        };
    }
}