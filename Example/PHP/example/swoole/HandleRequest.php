<?php


namespace example\swoole;
//require_once AUTOLOAD_FILE_ALIAS;


use example\UserManagerment;

class HandleRequest
{
    private $user = null;
    public function __construct()
    {
        $this->user = new UserManagerment();
    }

    public function onReceiveTcp($serv, $fd, $reactor_id, $data)
    {
        if(isset($_GET['name'])){
            $user = $_GET['name'];
        }else{
            $user = $data;
        }

        $this->user->checkUser($user);

        $this->user->register($user);

        $serv->send($fd, 'Swoole: '.$data);
        $serv->close($fd);
    }

    public function onReceiveHttp($request, $response){
        if(isset($_GET['name'])){
            $user = $_GET['name'];
        }else{
            $user = $request->get["name"];
        }

        $this->user->checkUser($user);

        $this->user->register($user);

        $response->end("<h1>Hello Swoole. #".$user."</h1>");
    }
}