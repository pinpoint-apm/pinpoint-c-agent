<?php


namespace app;


class TestFinalFunc
{
    public final function test($a) {
        echo "Final function test() called\n";
        return $a;
    }
}