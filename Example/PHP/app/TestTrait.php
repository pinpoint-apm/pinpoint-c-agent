<?php


namespace app;

class TestTrait
{
    use FuncInTrait;

    public function test(){
        echo "Func not in trait! ";
    }
}