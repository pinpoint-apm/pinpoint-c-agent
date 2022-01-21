<?php


namespace app\common;
use app\common\Person;

class Student extends Person
{
    function eat()
    {
        echo "Student eating<br/>";
        return "Student";
    }
}