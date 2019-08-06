<?php


namespace app;
use Exception;

class TestError
{
    public $num = 10;
    public $name = "sam";
    static function throwException()
    {
        throw new \Exception("throw some thing");
    }

    static function error()
    {
        xxx(234.343,456345,"fad",[1,2,3]);
    }

    static function test_cumsum_e1($n)
    {
        if ($n < 1) throw new Exception("n < 1");
        $tmp = $n + static::test_cumsum_e1($n-1);
    }

    public function testRetParm(&$arg){
        //        unset($arg);
        $arg = "bbbb";
        echo "aaaaaaa";
        return $arg;
    }

    public function testRetConst(&$arg){
        //        unset($arg);
        $arg = "bbbb";
        echo "aaaaaaa";
        return "adfadf";
    }

    public function testRetConstAndExp(&$arg){
        //        unset($arg);
        $arg = "bbbb";
        throw new Exception("sssssssssssssss");
        echo "aaaaaaa";
        return "adfadf";
    }

    public function testRetArgAndExp(&$arg){
        //        unset($arg);
        $arg = "bbbb";
        throw new Exception("sssssssssssssss");
        echo "aaaaaaa";
        return $arg[0];
    }

    public function Strict(&$arg){
        echo "Test";
        print_r($arg);
    }


    private function test_cumsum1($n)
    {
        if ($n == 1) return 1;
        return $n + $this->test_cumsum1($n - 1);
    }

    private function test_cumsum2($n)
    {
        if ($n == 1) return 1;
        return $n + $this->test_cumsum2($n - 1);
    }

    private function test_cumsum3($n)
    {
        if ($n < 1) throw new \Exception("n < 1");
        $tmp = $n;
        try
        {
            $tmp += $this->test_cumsum3($n-1);
        }
        catch( Exception $e)
        {
        }
        return $tmp;
    }

    private function test_cumsum4($n)
    {
        if ($n < 1) throw new Exception("n < 1");
        $tmp = $n;
        try
        {
            $tmp += $this->test_cumsum4($n-1);
        }
        catch( Exception $e)
        {
        }
        return $tmp;
    }

    public function testExpInDeepLevel(){
        echo $this->test_cumsum1(3);
        echo $this->test_cumsum2(3);
        echo $this->test_cumsum3(3);
        echo $this->test_cumsum4(3);
    }
}
