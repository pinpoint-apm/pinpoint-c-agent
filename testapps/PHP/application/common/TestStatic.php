<?php


namespace app\common;


class TestStatic
{
    public static function static_func($x) {
        echo "Test";
        return $x+1;
    }
}