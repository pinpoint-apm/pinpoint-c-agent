<?php


namespace app;
use app\Person;

class Student extends Person
{
    function eat()
    {
        echo "Student eating<br/>";
        return "Student";
    }
}