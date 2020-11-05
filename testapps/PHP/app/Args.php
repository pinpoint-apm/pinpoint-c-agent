<?php

define("GREETING","Hello you! How are you today?");

function xxxx()
{
    echo "xxxx";
}

class SObject
{
    var $x_;

    public function __construct($x)
    {
        $this->x_ = $x;
    }
};

class SObjectString
{
    var $x_;

    public function __construct($x)
    {
        $this->x_ = $x;
    }

    public function __toString()
    {
        return $this->x_;
    }

};
