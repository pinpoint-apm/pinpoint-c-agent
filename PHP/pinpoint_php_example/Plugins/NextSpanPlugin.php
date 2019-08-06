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
use Plugins\PerRequestPlugins;

///@hook:app\AccessRemote::\curl_setopt
///@hook:app\AccessRemote::\curl_exec
///@hook:app\AccessRemote::\curl_close
class NextSpanPlugin extends Candy
{
    function onBefore()
    {
        $argv = &$this->args[0];
        if( isset($argv[1]) && ($argv[1] == CURLOPT_HTTPHEADER)){
            $ch = $argv[0];
            // check trace limited
            // 1. Not send trace to collector-agent
            // 2. Pass s0 to downstream
            if(PerRequestPlugins::instance()->traceLimit()){
                $argv[2][] = 'Pinpoint-Sampled:s0';
                return ;
            }

            $argv[2][] ='Pinpoint-Sampled:s1';
            $argv[2][] ='Pinpoint-Flags:0';
            $argv[2][] ='Pinpoint-Papptype:1500';
            $argv[2][] ='Pinpoint-Pappname:'.pinpoint_app_name();

            $argv[2][] = 'Pinpoint-Host:'.$this->gethostFromCh($ch);

            $argv[2][] ='Pinpoint-Traceid:'.PerRequestPlugins::instance()->tid;
            $argv[2][] ='Pinpoint-Pspanid:'.PerRequestPlugins::instance()->sid;
            $nsid = PerRequestPlugins::instance()->generateSpanID();
            $argv[2][] ='Pinpoint-Spanid:'.$nsid;

            pinpoint_add_clues(PHP_ARGS,"headers");
            pinpoint_add_clue("stp",PHP_METHOD);

            return ;
        }


    }

    function onEnd(&$ret)
    {
        if($this->apId == 'curl_exec'){
            $argv = &$this->args[0];
            $ch = $argv[0];
            pinpoint_add_clue("dst",$this->gethostFromCh($ch));
            pinpoint_add_clue("stp",PINPOINT_PHP_REMOTE);
            pinpoint_add_clue('nsid',PerRequestPlugins::instance()->getCurNextSpanId());
            pinpoint_add_clues(HTTP_URL,curl_getinfo($ch,CURLINFO_EFFECTIVE_URL));
            pinpoint_add_clues(HTTP_STATUS_CODE,curl_getinfo($ch,CURLINFO_HTTP_CODE));

            //todo http io , while curl not support get the time usage
            //pinpoint_add_clues(HTTP_IO,sprintf("11:[%d,%d,%d,%d]",);
        }
    }

    function onException($e)
    {

    }

    function gethostFromCh($ch)
    {
        $URL   = parse_url(curl_getinfo($ch,CURLINFO_EFFECTIVE_URL));
        if(isset($URL['port']))
        {
            return $URL['host'].":".$URL['port'];
        }else{
            return $URL['host'];
        }
    }
}
