<?php


namespace example\workerman\Plugins;

use Workerman;
use example\workerman\Plugins\Candy;
use Workerman\Connection\TcpConnection;
require_once "PluginsDefines.php";

///@hook:example\workerman\HandleRequest::onMessage
class TcpServerPerRequestPlugin extends Candy
{
    public static $_intance = null;
    public $tid = null;
    public $sid = null;
    public $psid = null;
    public $pname = null;
    public $ptype = null;
    public $ah = null;
    private $curNextSpanId ='';
    private $isLimit =false;

    public function traceLimit()
    {
        return $this->isLimit;
    }

    public static function instance()
    {
        if (self::$_intance == null)
        {
            self::$_intance = new PerRequestPlugin();
        }
        return self::$_intance;
    }


    public function onBefore()
    {
        $connection = $this->args[0];
        assert($connection instanceof Workerman\Connection\TcpConnection);
        var_dump($connection->getRemoteIp());

        pinpoint_add_clue("uri","tcp connection");
        pinpoint_add_clue("client",$connection->getRemoteIp());
        pinpoint_add_clue("server",sprintf("%s:%d", $connection->getLocalIp(),$connection->getLocalPort()));
        pinpoint_add_clue("stp",PHP);
        pinpoint_add_clue("name","TCP Request");

        if(defined('APPLICATION_NAME')){
            pinpoint_add_clue("appname",APPLICATION_NAME);
            $this->app_name = APPLICATION_NAME;
        }else{
            $this->app_name = pinpoint_app_name();
        }

        if(defined('APPLICATION_ID'))
        {
            pinpoint_add_clue('appid',APPLICATION_ID);
            $this->app_id = APPLICATION_ID;
        }else{
            $this->app_id = pinpoint_app_id();
        }

        $this->sid = $this->generateSpanID();
        $this->tid = $this->generateTransactionID();

        pinpoint_add_clue("tid",$this->tid);
        pinpoint_add_clue("sid",(string)$this->sid);
    }

    public function onEnd(&$ret)
    {
        // reset limit
        $this->isLimit = false;
    }

    public function generateSpanID()
    {
        try
        {
            $this->curNextSpanId = mt_rand();//random_int(-99999999,99999999);
            return $this->curNextSpanId;
        }catch (\Exception $e)
        {
            return rand();
        }
    }

    public function getCurNextSpanId()
    {
        return $this->curNextSpanId;
    }

    public function generateTransactionID()
    {
        return  pinpoint_app_id().'^'.strval(pinpoint_start_time()).'^'.strval(pinpoint_unique_id());
    }


    function onException($e)
    {
        // TODO: Implement onException() method.
    }
}