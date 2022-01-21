<?php


namespace app\common;
use app\common\AbstractPerson;

class AbstractStudent extends AbstractPerson
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