<?php


namespace app\common;
use app\common\Person;

class Student extends Person
{
    function eat()
    {
        return "Student eating<br/>";
    }
}