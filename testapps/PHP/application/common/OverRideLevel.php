<?php


namespace app\common;

#app\OverRideLevel::public_function app\OverRideLevel::protected_function function app\OverRideLevel::private_function
class OverRideLevel extends LevelClass
{
    protected function protected_function($a)
    {
        $tmp = parent::protected_function($a);
        return "Protected func in subclass".$a;
    }

    public function public_function($a)
    {
        $tmp1 = $this->protected_function($a);
        $tmp2 = parent::public_function($a);
        return "Public func in subclass".$a;
    }
}