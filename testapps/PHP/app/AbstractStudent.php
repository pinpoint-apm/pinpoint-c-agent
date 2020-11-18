<?php


namespace app;
use app\AbstractPerson;

class AbstractStudent extends \app\AbstractPerson
{
    protected $name;
    function __construct($name)
    {
        $this->name = $name;
    }
    function eat(){
        return $this->name." is eating.<br/>";
    }
    function drink(){
        return $this->name." is drinking.<br/>";
    }

//    function breath(){
//        return $this->name." is breathing.<br/>";
//    }
}