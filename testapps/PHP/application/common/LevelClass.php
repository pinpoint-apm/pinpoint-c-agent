<?php


namespace app\common;

#app\LevelClass::public_function app\LevelClass::protected_function function app\LevelClass::private_function
class LevelClass
{
    private function private_function($a){
        return "Private!".$a;
    }

    protected function protected_function($a){
        $tmp = $this->private_function(123);
        return "Protected!".$a;
    }

    public function public_function($a){
        $tmp1 = $this->private_function(123);
        $tmp2 = $this->protected_function(123);
        return "Public!".$a;
    }
}