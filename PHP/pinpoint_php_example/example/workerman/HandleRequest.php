<?php
namespace example\workerman;
use example\UserManagerment;

class HandleRequest
{
    private $user = null;
    public function __construct()
    {
        $this->user = new UserManagerment();
    }

    public function onMessage($connection, $data)
    {
        if(isset($_GET['name'])){
            $user = $_GET['name'];
        }else{
            $user = $data;
        }

        if(!$this->user->checkUser($user))
        {
            $this->user->register($user);
        }

        $connection->send("hello ".$user." \n");
    }
}