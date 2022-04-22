<?php


namespace app\common;

class TestTrait
{
    use FuncInTrait;

    public function test(){
        return "Func not in trait! ";
    }
}