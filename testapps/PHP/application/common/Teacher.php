<?php


namespace app\common;


class Teacher extends Person
{
    function eat()
    {
        echo parent::eat(); // TODO: Change the autogenerated stub
        echo "Teacher eating too!<br/>";
        return "teacher";
    }
}