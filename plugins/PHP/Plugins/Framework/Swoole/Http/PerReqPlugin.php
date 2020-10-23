<?php
namespace Plugins\Framework\Swoole\Http;

use Plugins\Framework\Swoole\IDContext;
use Plugins\Util\Trace;
require_once __DIR__."/../../../__init__.php";

class PerReqPlugin
{
    private  $_callback;
    public $tid = null;
    public $sid = null;
    public $psid = null;
    public $pname = null;
    public $ptype = null;

    public $ah = null;
    public $app_name = null;
    public $app_id = null;
    private $curNextSpanId = '';
    public $request;
    public $reponse;

    public function __construct(callable $callback)
    {
        $this->_callback = $callback;

    }
    
    protected function onBefore()
    {
        $id = pinpoint_start_trace(0);
        IDContext::set($id);
        print("PerReqPlugin call onBefore ".IDContext::get()."\n");
        $request =$this->request;
        $header = &$request->header;
        pinpoint_add_clue(PP_REQ_URI,$request->server['request_uri'],$id);
        pinpoint_add_clue(PP_REQ_CLIENT,$request->server['remote_addr'],$id);
        pinpoint_add_clue(PP_REQ_SERVER,$header['host'],$id);
        pinpoint_add_clue(PP_SERVER_TYPE, PP_PHP,$id);
        pinpoint_add_clue(PP_INTERCEPTER_NAME, "swoole-http-server",$id);
        $this->app_name = APPLICATION_NAME;
        pinpoint_add_clue(PP_APP_NAME, $this->app_name,$id);
        pinpoint_set_context(PP_APP_NAME,$this->app_name,$id);
        $this->app_id = APPLICATION_ID;
        pinpoint_add_clue(PP_APP_ID, $this->app_id,$id);
        pinpoint_set_context(PP_APP_NAME, $this->app_id,$id);

        if (isset($header['HTTP_PINPOINT_PSPANID']) || array_key_exists("HTTP_PINPOINT_PSPANID", $header)) {
            $this->psid = $header['HTTP_PINPOINT_PSPANID'];
            pinpoint_add_clue(PP_PARENT_SPAN_ID, $this->psid,$id);
            pinpoint_set_context(PP_PARENT_SPAN_ID, $this->psid,$id);
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

            pinpoint_add_clue(PP_PARENT_NAME, $this->pname,$id);
            echo "pname: $this->pname";
        }

        if (isset($header['HTTP_PINPOINT_PAPPTYPE']) || array_key_exists("HTTP_PINPOINT_PAPPTYPE", $header)) {
            $this->ptype = $header['HTTP_PINPOINT_PAPPTYPE'];
            pinpoint_add_clue(PP_PARENT_TYPE, $this->ptype,$id);
        }

        if (isset($header['HTTP_PINPOINT_HOST']) || array_key_exists("HTTP_PINPOINT_HOST", $header)) {
            $this->ah = $header['HTTP_PINPOINT_HOST'];
            pinpoint_add_clue(PP_PARENT_HOST, $this->ah,$id);
        }
        if (isset($header[PP_NGINX_PROXY]) || array_key_exists(PP_NGINX_PROXY, $header)) {
            pinpoint_add_clue(PP_NGINX_PROXY, $header[PP_NGINX_PROXY],$id);
        }

        if (isset($header[PP_APACHE_PROXY]) || array_key_exists(PP_APACHE_PROXY, $header)) {
            pinpoint_add_clue(PP_APACHE_PROXY, $header[PP_APACHE_PROXY],$id);
        }
        pinpoint_set_context("Pinpoint-Sampled",PP_SAMPLED,$id);
        if (isset($header[PP_SAMPLED]) || array_key_exists(PP_SAMPLED, $header)) {
            if ($header[PP_SAMPLED] == PP_NOT_SAMPLED) {
                //drop this request. collector could not receive any thing
                pinpoint_drop_trace($id);
                pinpoint_set_context("Pinpoint-Sampled",PP_NOT_SAMPLED,$id);
            }

        } else {
            if(pinpoint_tracelimit())
            {
                pinpoint_set_context("Pinpoint-Sampled",PP_NOT_SAMPLED,$id);
                pinpoint_drop_trace($id);
            }
        }

        pinpoint_add_clue(PP_TRANSCATION_ID, $this->tid,$id);
        pinpoint_set_context(PP_TRANSCATION_ID, $this->tid,$id);
        pinpoint_add_clue(PP_SPAN_ID, $this->sid,$id);
        pinpoint_set_context(PP_SPAN_ID, (string)$this->sid,$id);

        pinpoint_set_context((string)PP_HTTP_STATUS_CODE,'200',$id);

    }


    protected function onException($e)
    {

    }

    protected function onEnd(&$ret)
    {
        $id = IDContext::get();
        $code = pinpoint_get_context((string)PP_HTTP_STATUS_CODE,$id);
        pinpoint_add_clues(PP_HTTP_STATUS_CODE,$code,$id,PINPOINT_ROOT_LOC);
        pinpoint_end_trace($id);
        print("PerReqPlugin call onEnd ".IDContext::get()."\r\n\r\n");
    }

    public function __invoke(&...$args)
    {
        $this->request = &$args[0];
        $this->reponse = &$args[1];
        $this->onBefore();
        try{
//            $ret = call_user_func_array($this->_callback,[&$this->request,new Response($this->reponse)]);
            $ret = call_user_func_array($this->_callback,[&$this->request,&$this->reponse]);
            $this->onEnd($ret);
            return $ret;
        }catch (\Exception $e){
            $this->onException($e);
            throw $e;
        }
    }

    public function generateTransactionID()
    {
        return  $this->app_id . '^' . strval(pinpoint_start_time()) . '^' . strval(pinpoint_unique_id());
    }

}