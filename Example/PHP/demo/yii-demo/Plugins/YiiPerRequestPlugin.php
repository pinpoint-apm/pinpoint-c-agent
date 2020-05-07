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

use Plugins\Candy;
require_once "PluginsDefines.php";

///@hook:yii\base\Application::run
class YiiPerRequestPlugin extends Candy
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
        pinpoint_add_clue("uri","yii web app");
        pinpoint_add_clue("client",$_SERVER["REMOTE_ADDR"]);
        pinpoint_add_clue("server",$_SERVER["HTTP_HOST"]);
        pinpoint_add_clue("stp",PHP);
        pinpoint_add_clue("name","Web Request");

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
        return  $this->app_id.'^'.strval(pinpoint_start_time()).'^'.strval(pinpoint_unique_id());
    }


    function onException($e)
    {
        // TODO: Implement onException() method.
    }
}