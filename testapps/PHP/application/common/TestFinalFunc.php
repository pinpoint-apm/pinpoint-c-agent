<?php


namespace app\common;


class TestFinalFunc
{
    public final function test($a) {
        return "Final function test() called\n".$a;
    }
}