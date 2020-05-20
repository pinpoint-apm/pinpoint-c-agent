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

class NextSpanPlugin extends Candy
{

///@hook:Laminas\Diactoros\Response\RedirectResponse::__construct
    public function onBefore(){
/*
        $headers = &$this->args[2];
        if(PerRequestPlugins::instance()->traceLimit()){
            $headers['Pinpoint-Sampled'] = 's0';
            return ;
        }
        $headers['Pinpoint-Sampled'] ='s1';
        $headers['Pinpoint-Flags'] ='0';
        $headers['Pinpoint-Papptype'] ='1500';
        $headers['Pinpoint-Pappname'] =PerRequestPlugins::instance()->app_name;

        $headers['Pinpoint-Host'] = $this->args[0];
        $headers['Pinpoint-Traceid'] =PerRequestPlugins::instance()->tid;
        $headers['Pinpoint-Pspanid'] =PerRequestPlugins::instance()->sid;
        $nsid = PerRequestPlugins::instance()->generateSpanID();
        $headers['Pinpoint-Spanid'] =$nsid;
*/
        pinpoint_add_clue("dst",$this->getHostFromURL($this->args[0]));
        pinpoint_add_clue("stp",PINPOINT_PHP_REMOTE);
        pinpoint_add_clue('nsid',PerRequestPlugins::instance()->getCurNextSpanId());
        pinpoint_add_clues(HTTP_URL,parse_url($this->args[0],PHP_URL_PATH));
    }

    public function onEnd(&$ret){
        pinpoint_add_clues(HTTP_STATUS_CODE, string($this->who->getStatusCode()));
    }

    public function onException($e){
        pinpoint_add_clue("EXP",$e->getMessage());
    }

    function getHostFromURL(string $url)
    {
        $urlAr   = parse_url($url);
        $retUrl = '';
        if(isset($urlAr['host']))
        {
            $retUrl.=$urlAr['host'];
        }

        if(isset($urlAr['path'])){
            $retUrl.=$urlAr['path'];
        }

        if(isset($urlAr['port']))
        {
            $retUrl .= ":".$urlAr['port'];
        }

        return $retUrl;
    }
}
