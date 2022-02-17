<?php


namespace app\common;


class Teacher extends Person
{
    function eat()
    {
        $tmp = parent::eat();
        return $tmp."Teacher eating too!<br/>";
    }
}