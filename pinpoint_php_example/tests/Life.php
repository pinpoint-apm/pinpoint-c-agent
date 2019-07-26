<?php


namespace Foo;


class Life
{
    public function __construct()
    {
        echo "Life start \n";
//        throw new \Exception("fvgbh");
    }
    public function do()
    {
        echo "fghjl \n";
    }
    public function __destruct()
    {
        echo "Life end \n";
    }
}

class Test
{
    private static $instance=null;
    public static  function create(){
        if(Test::$instance ){
            return Test::$instance;
        }
        Test::$instance = new Test();

    }
    private function __construct()
    {
        echo "__construct";
    }
    public function __destruct()
    {
       echo "__destruct";
    }
}

//static $life = new Life();
//return $life;
