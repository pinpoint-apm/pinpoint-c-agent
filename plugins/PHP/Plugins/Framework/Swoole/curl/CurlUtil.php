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

use Plugins\Framework\Swoole\IDContext;
use Plugins\Util\Trace;

class CurlUtil
{
    public static function appendPinpointHeader($url, &$headers)
    {
        if(pinpoint_get_context('Pinpoint-Sampled', IDContext::get())==PP_NOT_SAMPLED){
            $headers[] = 'Pinpoint-Sampled:s0';
            return ;
        }

        $headers[] ='Pinpoint-Sampled:s1';
        $headers[] ='Pinpoint-Flags:0';
        $headers[] ='Pinpoint-Papptype:1500';
        $headers[] ='Pinpoint-Pappname:'.APPLICATION_NAME;

        $headers[] = 'Pinpoint-Host:'.static::getHostFromURL($url);

        $headers[] ='Pinpoint-Traceid:'.pinpoint_get_context(PP_TRANSCATION_ID, IDContext::get());
        $headers[] ='Pinpoint-Pspanid:'.pinpoint_get_context(PP_SPAN_ID, IDContext::get());
        $nsid = Trace::generateSpanID();
        $headers[] ='Pinpoint-Spanid:'.$nsid;
        pinpoint_set_context(PP_NEXT_SPAN_ID, (string)$nsid, IDContext::get());
    }

    public static function appendPinpointHeaderInKeyValue($url, &$headers)
    {
        if(pinpoint_get_context('Pinpoint-Sampled', IDContext::get())==PP_NOT_SAMPLED){
            $headers['Pinpoint-Sampled'] =PP_NOT_SAMPLED;
            return ;
        }

        $headers['Pinpoint-Sampled'] =PP_SAMPLED;
        $headers['Pinpoint-Flags'] ='0';
        $headers['Pinpoint-Papptype'] ='1500';
        $headers['Pinpoint-Pappname'] = APPLICATION_NAME;

        $headers['Pinpoint-Host'] = static::getHostFromURL($url);

        $headers['Pinpoint-Traceid'] = pinpoint_get_context(PP_TRANSCATION_ID, IDContext::get());
        $headers['Pinpoint-Pspanid'] = pinpoint_get_context(PP_SPAN_ID, IDContext::get());
        $nsid = Trace::generateSpanID();
        $headers['Pinpoint-Spanid'] = $nsid;
        pinpoint_set_context(PP_NEXT_SPAN_ID, (string)$nsid, IDContext::get());
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