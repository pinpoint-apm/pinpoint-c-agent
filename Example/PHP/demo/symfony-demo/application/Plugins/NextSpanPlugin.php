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


///@hook:Symfony\Component\HttpClient\AmpHttpClient::request
///@hook:Symfony\Component\HttpClient\CurlHttpClient::request
///@hook:Symfony\Component\HttpClient\NativeHttpClient::request
class NextSpanPlugin extends Candy
{
    public function onBefore(){
        pinpoint_add_clue("dst",$this->getHostFromURL($this->args[1]));
        pinpoint_add_clue("stp",PINPOINT_PHP_REMOTE);
        pinpoint_add_clue('nsid',PerRequestPlugins::instance()->getCurNextSpanId());
        pinpoint_add_clues(HTTP_URL,parse_url($this->args[1],PHP_URL_PATH));
    }

    public function onEnd(&$ret){
        pinpoint_add_clues(HTTP_STATUS_CODE, (string)$ret->getStatusCode());
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