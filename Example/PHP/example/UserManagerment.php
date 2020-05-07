<?php


namespace example;


class UserManagerment
{
    public function checkUser($name)
    {
        return false;
    }
    public function register($name)
    {
        //do something
        $this->cacheUser($name);
        return true;
    }
    public function cacheUser($name)
    {
        return true;
    }
}