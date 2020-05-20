<?php


namespace app;

#app\LevelClass::public_function app\LevelClass::protected_function function app\LevelClass::private_function
class LevelClass
{
    private function private_function($a){
        echo "Private!";
        return $a;
    }

    protected function protected_function($a){
        echo "Protected!";
        $this->private_function(123);
        return $a;
    }

    public function public_function($a){
        echo "Public!";
        $this->private_function(123);
        $this->protected_function(123);
        return $a;
    }
}