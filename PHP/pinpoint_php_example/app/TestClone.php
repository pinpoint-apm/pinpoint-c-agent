<?php


namespace app;


class TestClone
{
    protected $_color = 'black';
    protected $name = "ChangHong";
    public function setColor($color){
        $this->_color = $color;
    }
    /**
     * @return string
     */
    public function getColor()
    {
        return $this->name." color is ".$this->_color;
    }
    public function __clone()
    {
        $this->name = "ClonableName";
    }
}