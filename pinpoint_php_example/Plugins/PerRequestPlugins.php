<?php
#-------------------------------------------------------------------------------
# Copyright 2019 NAVER Corp
# 
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License.  You may obtain a copy
# of the License at
# 
#   http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations under
# the License.
#-------------------------------------------------------------------------------


namespace Plugins;
require_once "PluginsDefines.php";

class PerRequestPlugins
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
            self::$_intance = new PerRequestPlugins();
        }
        return self::$_intance;
    }

    private function initTrace()
    {
        while (pinpoint_end_trace() >0);
        $this->isLimit =pinpoint_tracelimit();
    }

    private function __construct()
    {
        $this->initTrace();

        pinpoint_start_trace();
        pinpoint_add_clue("uri",$_SERVER['REQUEST_URI']);
        pinpoint_add_clue("client",$_SERVER["REMOTE_ADDR"]);
        pinpoint_add_clue("server",$_SERVER["HTTP_HOST"]);
        pinpoint_add_clue("stp",PHP);
        pinpoint_add_clue("name","PHP Request");

        if(isset($_SERVER['HTTP_PINPOINT_PSPANID']) || array_key_exists("HTTP_PINPOINT_PSPANID",$_SERVER))
        {
            $this->psid = $_SERVER['HTTP_PINPOINT_PSPANID'];
            pinpoint_add_clue("psid",$this->psid);
            echo "psid: $this->psid \n";
        }

        if(isset($_SERVER['HTTP_PINPOINT_SPANID']) || array_key_exists("HTTP_PINPOINT_SPANID",$_SERVER))
        {
            $this->sid = $_SERVER['HTTP_PINPOINT_SPANID'];
            echo "sid: $this->sid \n";
        }else{
            $this->sid = $this->generateSpanID();
        }

        if(isset($_SERVER['HTTP_PINPOINT_TRACEID']) || array_key_exists("HTTP_PINPOINT_TRACEID",$_SERVER))
        {
            $this->tid = $_SERVER['HTTP_PINPOINT_TRACEID'];
            echo "tid: $this->tid\n";
        }else{
            $this->tid = $this->generateTransactionID();
        }

        if(isset($_SERVER['HTTP_PINPOINT_PAPPNAME']) || array_key_exists("HTTP_PINPOINT_PAPPNAME",$_SERVER))
        {
            $this->pname = $_SERVER['HTTP_PINPOINT_PAPPNAME'];

            pinpoint_add_clue('pname',$this->pname);
            echo "pname: $this->pname";
        }

        if(isset($_SERVER['HTTP_PINPOINT_PAPPTYPE']) || array_key_exists("HTTP_PINPOINT_PAPPTYPE",$_SERVER))
        {
            $this->ptype = $_SERVER['HTTP_PINPOINT_PAPPTYPE'];
            pinpoint_add_clue('ptype',$this->ptype);
            echo "ptype: $this->pname";
        }

        if(isset($_SERVER['HTTP_PINPOINT_HOST']) || array_key_exists("HTTP_PINPOINT_HOST",$_SERVER))
        {
            $this->ah = $_SERVER['HTTP_PINPOINT_HOST'];
            pinpoint_add_clue('Ah',$this->ah);
            echo "Ah: $this->ah";
        }
        if(isset($_SERVER[NGINX_PROXY]) ||array_key_exists(NGINX_PROXY,$_SERVER))
        {
            pinpoint_add_clue("NP",$_SERVER[NGINX_PROXY]);
        }

        if(isset($_SERVER[APACHE_PROXY]) ||array_key_exists(APACHE_PROXY,$_SERVER))
        {
            pinpoint_add_clue("AP",$_SERVER[APACHE_PROXY]);
        }

        pinpoint_add_clue("tid",$this->tid);
        pinpoint_add_clue("sid",$this->sid);

    }
    public function __destruct()
    {
        pinpoint_add_clues(HTTP_STATUS_CODE,http_response_code());
        pinpoint_end_trace();
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

}
