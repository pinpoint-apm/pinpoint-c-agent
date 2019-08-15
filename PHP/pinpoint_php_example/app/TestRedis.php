<?php


namespace app;

use \Redis;


class TestRedis
{
    private $rd;

    public function connect_redis(){
        $this->rd = new Redis();
        $this->rd->connect('localhost', 6379);
        echo $this->rd->ping();
    }

    public function getdata($key){
        return $this->rd->get($key);
    }

    public function getkey(){
        return $this->rd->keys('*');
    }

    public function setdata($key, $value){
        $this->rd->set($key, $value);
    }

    public function deldata($key){
        $this->rd->del($key);
    }
}