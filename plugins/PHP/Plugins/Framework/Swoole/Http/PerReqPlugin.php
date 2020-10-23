<?php
/******************************************************************************
 * Copyright 2020 NAVER Corp.                                                 *
 *                                                                            *
 * Licensed under the Apache License, Version 2.0 (the "License");            *
 * you may not use this file except in compliance with the License.           *
 * You may obtain a copy of the License at                                    *
 *                                                                            *
 *     http://www.apache.org/licenses/LICENSE-2.0                             *
 *                                                                            *
 * Unless required by applicable law or agreed to in writing, software        *
 * distributed under the License is distributed on an "AS IS" BASIS,          *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   *
 * See the License for the specific language governing permissions and        *
 * limitations under the License.                                             *
 ******************************************************************************/
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

        if (isset($header[PP_HEADER_PSPANID]) || array_key_exists(PP_HEADER_PSPANID, $header)) {
            $this->psid = $header[PP_HEADER_PSPANID];
            pinpoint_add_clue(PP_PARENT_SPAN_ID, $this->psid,$id);
            pinpoint_set_context(PP_PARENT_SPAN_ID, $this->psid,$id);
        }

        if (isset($header[PP_HEADER_SPANID]) || array_key_exists(PP_HEADER_SPANID, $header)) {
            $this->sid = $header[PP_HEADER_SPANID];
        } else {
            $this->sid = Trace::generateSpanID();
        }

        if (isset($header[PP_HEADER_TRACEID]) || array_key_exists(PP_HEADER_TRACEID, $header)) {
            $this->tid = $header[PP_HEADER_TRACEID];
        } else {
            $this->tid = $this->generateTransactionID();
        }

        if (isset($header[PP_HEADER_PAPPNAME]) || array_key_exists(PP_HEADER_PAPPNAME, $header)) {
            $this->pname = $header[PP_HEADER_PAPPNAME];

            pinpoint_add_clue(PP_PARENT_NAME, $this->pname,$id);
        }

        if (isset($header[PP_HEADER_PAPPTYPE]) || array_key_exists(PP_HEADER_PAPPTYPE, $header)) {
            $this->ptype = $header[PP_HEADER_PAPPTYPE];
            pinpoint_add_clue(PP_PARENT_TYPE, $this->ptype,$id);
        }

        if (isset($header[PP_HEADER_PINPOINT_HOST]) || array_key_exists(PP_HEADER_PINPOINT_HOST, $header)) {
            $this->ah = $header[PP_HEADER_PINPOINT_HOST];
            pinpoint_add_clue(PP_PARENT_HOST, $this->ah,$id);
        }
        if (isset($header[PP_HEADER_NGINX_PROXY]) || array_key_exists(PP_HEADER_NGINX_PROXY, $header)) {
            pinpoint_add_clue(PP_NGINX_PROXY, $header[PP_HEADER_NGINX_PROXY],$id);
        }

        if (isset($header[PP_HEADER_APACHE_PROXY]) || array_key_exists(PP_HEADER_APACHE_PROXY, $header)) {
            pinpoint_add_clue(PP_APACHE_PROXY, $header[PP_HEADER_APACHE_PROXY],$id);
        }
        pinpoint_set_context("Pinpoint-Sampled",PP_SAMPLED,$id);
        if (isset($header[PP_HEADER_SAMPLED]) || array_key_exists(PP_HEADER_SAMPLED, $header)) {
            if ($header[PP_HEADER_SAMPLED] == PP_NOT_SAMPLED) {
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
        pinpoint_add_clues(PP_HTTP_STATUS_CODE,$code,$id,PP_ROOT_LOC);
        pinpoint_end_trace($id);
    }

    public function __invoke(&...$args)
    {
        $this->request = &$args[0];
        $this->reponse = &$args[1];
        $this->onBefore();
        try{
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