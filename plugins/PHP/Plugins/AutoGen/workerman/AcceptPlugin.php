<?php
/**
 * Created by PhpStorm.
 * User: eeliu
 * Date: 10/14/20
 * Time: 2:28 PM
 */

namespace Plugins\AutoGen\workerman;
require_once __DIR__."/../../__init__.php";
use Plugins\Util\Trace;

/**
 * @hook: Workerman\Worker::acceptConnection
 */
class AcceptPlugin
{
    protected $worker_self;
    protected $origin_onMessage;
    protected $origin_onConnect;
    protected $origin_onClose;
    public function __construct($apId,$who,&...$args)
    {
        assert($who instanceof \Workerman\Worker);
        $this->worker_self = $who;
    }

    public function onEnd()
    {
        $this->worker_self->onMessage =  $this->origin_onMessage;
        $this->worker_self->onConnect =  $this->origin_onConnect;
        $this->worker_self->onClose   =  $this->origin_onClose ;
    }


    public function onBefore(){
        $this->origin_onMessage =  $this->worker_self->onMessage;
        $this->origin_onConnect =  $this->worker_self->onConnect;
        $this->origin_onClose  =   $this->worker_self->onClose;
        $this->worker_self->onMessage = function ($connection, $request) {
            $id = $connection->_pinpoint_node_id_;
            echo "onMessage $id \n";
            Context::getInstance()->setId($id);
            $this->onRequest($connection,$request);
            if(!empty($this->origin_onMessage))
            {
                ($this->origin_onMessage)($connection,$request);
            }
        };

        $this->worker_self->onConnect = function($connection) {
            $id =pinpoint_start_trace(0);
            //  store _pinpoint_id_ into connection
            $connection->_pinpoint_node_id_ = $id;
            echo "onConnect $id \n";
            if(!empty($this->origin_onConnect))
            {
                ($this->origin_onConnect)($connection);
            }
        };

        $this->worker_self->onClose = function($connection)
        {
            // request end
            if(!empty($this->origin_onClose))
            {
                ($this->origin_onClose)($connection);
            }
            $this->onClose($connection);
        };


    }

    public function onException($e)
    {

    }


    public function onRequest(&$connection, &$request)
    {
        $id = $connection->_pinpoint_node_id_;

        $header =$request->header();
//        var_dump($header);
        pinpoint_add_clue("uri",$request->uri(),$id);
        pinpoint_add_clue("client",$connection->getRemoteIp(),$id);
        pinpoint_add_clue("server",$header['host'],$id);
        pinpoint_add_clue("stp", PHP,$id);
        pinpoint_add_clue(INTERCEPTER_NAME, "workerman-http",$id);
        $this->app_name = APPLICATION_NAME;
        pinpoint_add_clue("appname", $this->app_name,$id);
        pinpoint_set_context("appname",$this->app_name,$id);
        $this->app_id = APPLICATION_ID;
        pinpoint_add_clue('appid', $this->app_id,$id);
        pinpoint_set_context('appname', $this->app_id,$id);

        if (isset($header['HTTP_PINPOINT_PSPANID']) || array_key_exists("HTTP_PINPOINT_PSPANID", $header)) {
            $this->psid = $header['HTTP_PINPOINT_PSPANID'];
            pinpoint_add_clue("psid", $this->psid,$id);
            pinpoint_set_context('psid', $this->psid,$id);
            echo "psid: $this->psid \n";
        }

        if (isset($header['HTTP_PINPOINT_SPANID']) || array_key_exists("HTTP_PINPOINT_SPANID", $header)) {
            $this->sid = $header['HTTP_PINPOINT_SPANID'];
            echo "sid: $this->sid \n";
        } else {
            $this->sid = Trace::generateSpanID();
        }

        if (isset($header['HTTP_PINPOINT_TRACEID']) || array_key_exists("HTTP_PINPOINT_TRACEID", $header)) {
            $this->tid = $header['HTTP_PINPOINT_TRACEID'];
        } else {
            $this->tid = $this->generateTransactionID();
        }

        if (isset($header['HTTP_PINPOINT_PAPPNAME']) || array_key_exists("HTTP_PINPOINT_PAPPNAME", $header)) {
            $this->pname = $header['HTTP_PINPOINT_PAPPNAME'];

            pinpoint_add_clue('pname', $this->pname,$id);
            echo "pname: $this->pname";
        }

        if (isset($header['HTTP_PINPOINT_PAPPTYPE']) || array_key_exists("HTTP_PINPOINT_PAPPTYPE", $header)) {
            $this->ptype = $header['HTTP_PINPOINT_PAPPTYPE'];
            pinpoint_add_clue('ptype', $this->ptype,$id);
        }

        if (isset($header['HTTP_PINPOINT_HOST']) || array_key_exists("HTTP_PINPOINT_HOST", $header)) {
            $this->ah = $header['HTTP_PINPOINT_HOST'];
            pinpoint_add_clue('Ah', $this->ah,$id);
        }
        if (isset($header[NGINX_PROXY]) || array_key_exists(NGINX_PROXY, $header)) {
            pinpoint_add_clue("NP", $header[NGINX_PROXY],$id);
        }

        if (isset($header[APACHE_PROXY]) || array_key_exists(APACHE_PROXY, $header)) {
            pinpoint_add_clue("AP", $header[APACHE_PROXY],$id);
        }

        pinpoint_set_context("Pinpoint-Sampled","s1",$id);
        if (isset($header[SAMPLED]) || array_key_exists(SAMPLED, $header)) {
            if ($header[SAMPLED] == "s0") {
                //drop this request. collector could not receive any thing
                pinpoint_drop_trace($id);
                pinpoint_set_context("Pinpoint-Sampled","s0",$id);
            }

        } else {
            if(pinpoint_tracelimit())
            {
                pinpoint_set_context("Pinpoint-Sampled","s0",$id);
                pinpoint_drop_trace($id);
            }
        }

        pinpoint_add_clue("tid", $this->tid,$id);
        pinpoint_set_context('tid', $this->tid,$id);
        pinpoint_add_clue("sid", $this->sid,$id);
        pinpoint_set_context('sid', $this->tid,$id);

    }

    public function onClose(&$connection)
    {
        pinpoint_end_trace($connection->_pinpoint_node_id_);
        echo "onClose ".$connection->_pinpoint_node_id_."\n";
    }

    public function generateTransactionID()
    {
        return  $this->app_id . '^' . strval(pinpoint_start_time()) . '^' . strval(pinpoint_unique_id());
    }
}