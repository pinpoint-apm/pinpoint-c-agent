<?php


namespace app\common;


class TestFinalFunc
{
    public final function test($a) {
        echo "Final function test() called\n";
        return $a;
    }
}