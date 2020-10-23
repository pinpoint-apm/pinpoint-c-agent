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

namespace Plugins\Framework\Swoole\curl;

use Plugins\Framework\Swoole\Candy;

class NextSpanPlugin extends Candy
{
    public $nsid = null;

    function onBefore()
    {
        pinpoint_add_clues(PHP_ARGS,"...",$this->id);
    }

    function onEnd(&$ret)
    {
        if($this->apId == 'curl_exec'){
            $argv = &$this->args[0];
            $ch = $argv[0];
            pinpoint_add_clue(PP_DESTINATION,$this->getHostFromURL(curl_getinfo($ch,CURLINFO_EFFECTIVE_URL)),$this->id);
            pinpoint_add_clue(PP_SERVER_TYPE,PP_PHP_REMOTE,$this->id);
            pinpoint_add_clue(PP_NEXT_SPAN_ID,pinpoint_get_context(PP_NEXT_SPAN_ID,$this->id),$this->id);
            pinpoint_add_clues(PP_HTTP_URL,curl_getinfo($ch,CURLINFO_EFFECTIVE_URL),$this->id);

        }
    }

    function onException($e)
    {

    }
    function getHostFromURL(string $url)
    {
        $urlAr   = parse_url($url);
        $retUrl = '';
        if(isset($urlAr['host']))
        {
            $retUrl.=$urlAr['host'];
        }

//        if(isset($urlAr['path'])){
//            $retUrl.=$urlAr['path'];
//        }

        if(isset($urlAr['port']))
        {
            $retUrl .= ":".$urlAr['port'];
        }

        return $retUrl;
    }
}
