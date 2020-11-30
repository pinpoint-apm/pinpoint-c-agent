<?php


namespace app;


class TestRecursion
{
    public function test_cumsum1($n)
    {
        if ($n == 1) return 1;
        print '*';
        var_dump($this);
        print '*';
#        if ($n < 1) throw new \Exception("n < 1");
        return $n + $this->test_cumsum1($n - 1);
    }

    public function test_cumsum2($n)
    {
        if ($n < 1) throw new \Exception("n < 1");
        $tmp = $n;
//        try
//        {
        $tmp += $this->test_cumsum2($n-1);
//        }
//        catch( \Exception $e)
//        {
//            echo $e->getMessage();
//        }
        return $tmp;
    }
}