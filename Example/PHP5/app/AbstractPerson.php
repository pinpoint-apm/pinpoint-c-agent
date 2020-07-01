<?php


namespace app;


abstract class AbstractPerson
{
    protected $name;
//    abstract function eat();
    abstract function drink();
    function breath(){
        return $this->name." is alive.<br/>";
    }
}