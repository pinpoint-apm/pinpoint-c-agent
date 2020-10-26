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
namespace Plugins\Sys\curl;
use Plugins\PerRequestPlugins;

class CurlUtil
{
    public static function appendPinpointHeader($ch, &$headers)
    {
        if(PerRequestPlugins::instance()->traceLimit()){
            $headers[] = 'Pinpoint-Sampled:s0';
            return ;
        }

        $headers[] ='Pinpoint-Sampled:s1';
        $headers[] ='Pinpoint-Flags:0';
        $headers[] ='Pinpoint-Papptype:1500';
        $headers[] ='Pinpoint-Pappname:'.PerRequestPlugins::instance()->app_name;

        $headers[] = 'Pinpoint-Host:'.static::getHostFromURL(curl_getinfo($ch,CURLINFO_EFFECTIVE_URL));

        $headers[] ='Pinpoint-Traceid:'.PerRequestPlugins::instance()->tid;
        $headers[] ='Pinpoint-Pspanid:'.PerRequestPlugins::instance()->sid;
        $nsid = PerRequestPlugins::instance()->generateSpanID();
        $headers[] ='Pinpoint-Spanid:'.$nsid;
    }

    public static function addPinpointHeader($ch,$url)
    {
        if(PerRequestPlugins::instance()->traceLimit()){
            \curl_setopt($ch,CURLOPT_HTTPHEADER,array("Pinpoint-Sampled:s0"));
            return ;
        }

        $nsid = PerRequestPlugins::instance()->generateSpanID();
        $header = array(
            'Pinpoint-Sampled:s1',
            'Pinpoint-Flags:0',
            'Pinpoint-Papptype:1500',
            'Pinpoint-Pappname:'.PerRequestPlugins::instance()->app_name,
            'Pinpoint-Host:'.static::getHostFromURL($url),
            'Pinpoint-Traceid:'.PerRequestPlugins::instance()->tid,
            'Pinpoint-Pspanid:'.PerRequestPlugins::instance()->sid,
            'Pinpoint-Spanid:'.$nsid
        );
        \curl_setopt($ch,CURLOPT_HTTPHEADER,$header);
    }

    public static function getHostFromURL(string $url)
    {
        $urlAr   = parse_url($url);
        $retUrl = '';
        if(isset($urlAr['host']))
        {
            $retUrl.=$urlAr['host'];
        }

        if(isset($urlAr['port']))
        {
            $retUrl .= ":".$urlAr['port'];
        }

        return $retUrl;
    }

}