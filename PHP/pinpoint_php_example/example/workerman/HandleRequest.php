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
            if(!$this->user->checkUser($user))
            {
                $this->user->register($user);
            }
            $connection->send("hello ".$user." \n");
        }
        else{
            $connection->send("hello no user\n");
        }
    }
}