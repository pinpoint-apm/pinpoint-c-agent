<?php

namespace app\common;
use app\util\Foo;

class AopFunction
{
    function test_func1($arg1, $arg2)
    {
        return sprintf("this is test_func1: arg1=%s, arg2=%s", (string)$arg1, (string)$arg2);
    }

    function test_func2()
    {
        Foo::test_func_2("1",23,null);
    }
}
