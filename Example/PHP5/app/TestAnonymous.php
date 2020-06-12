<?php


namespace app;
use app\AopFunction;

class TestAnonymous
{
    public function createbell($time) {
        return function() use ($time) {

            echo "$time". "di di di  di ...";
            $func1 = new AopFunction();
//            try{
//                throw new \Exception("exception in the closure.");
//            }catch (\Exception $e){
//                echo $e->getMessage();
//            }
            echo $func1->test_func1("x", 3);
            return $time;
        };
    }
}