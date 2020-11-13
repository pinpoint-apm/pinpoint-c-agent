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
use Plugins\Util\Trace;

class CurlUtil
{
//    public static function appendPinpointHeader($ch, &$headers)
//    {
//        if(pinpoint_get_context('Pinpoint-Sampled')==PP_NOT_SAMPLED){
//            $headers[] = 'Pinpoint-Sampled:s0';
//            return ;
//        }
//
//        $headers[] ='Pinpoint-Sampled:s1';
//        $headers[] ='Pinpoint-Flags:0';
//        $headers[] ='Pinpoint-Papptype:1500';
//        $headers[] ='Pinpoint-Pappname:'.APPLICATION_NAME;
//
//        $headers[] = 'Pinpoint-Host:'.static::getHostFromURL(curl_getinfo($ch,CURLINFO_EFFECTIVE_URL));
//
//        $headers[] ='Pinpoint-Traceid:'.pinpoint_get_context(PP_TRANSCATION_ID);
//        $headers[] ='Pinpoint-Pspanid:'.pinpoint_get_context(PP_SPAN_ID);
//        $nsid =  Trace::generateSpanID();
//        $headers[] ='Pinpoint-Spanid:'.$nsid;
//        pinpoint_set_context(PP_NEXT_SPAN_ID, (string)$nsid);
//    }

    public static function getPinpointHeader($url)
    {
        if(pinpoint_get_context('Pinpoint-Sampled')==PP_NOT_SAMPLED){
            return ["Pinpoint-Sampled:s0"];
        }

        $nsid = Trace::generateSpanID();
        $header = [
            'Pinpoint-Sampled:s1',
            'Pinpoint-Flags:0',
            'Pinpoint-Papptype:1500',
            'Pinpoint-Pappname:'.APPLICATION_NAME,
            'Pinpoint-Host:'.static::getHostFromURL($url),
            'Pinpoint-Traceid:'.pinpoint_get_context(PP_TRANSCATION_ID),
            'Pinpoint-Pspanid:'.pinpoint_get_context(PP_SPAN_ID),
            'Pinpoint-Spanid:'.$nsid
        ];
        pinpoint_set_context(PP_NEXT_SPAN_ID, (string)$nsid);
        return $header;
    }

    // for GuzzleHttp header
    public static function getPPHeader($url)
    {
        if(pinpoint_get_context('Pinpoint-Sampled')==PP_NOT_SAMPLED){
            return ["Pinpoint-Sampled"=>"s0"];
        }

        $nsid = Trace::generateSpanID();
        $header = [
            'Pinpoint-Sampled'=>'s1',
            'Pinpoint-Flags'=>'0',
            'Pinpoint-Papptype'=>'1500',
            'Pinpoint-Pappname'=>APPLICATION_NAME,
            'Pinpoint-Host'=>static::getHostFromURL($url),
            'Pinpoint-Traceid'=>pinpoint_get_context(PP_TRANSCATION_ID),
            'Pinpoint-Pspanid'=>pinpoint_get_context(PP_SPAN_ID),
            'Pinpoint-Spanid'=>$nsid
        ];
        pinpoint_set_context(PP_NEXT_SPAN_ID, (string)$nsid);
        return $header;
    }

    /**
     *
     * url is very funny
     * example.com
     * www.example.com:8000
     * www.example.com
     * http://www.example.com
     *  total must be accept
     *
     * @param string $url
     * @return string
     */
    public static function getHostFromURL(string $url)
    {
        $urlAr   = parse_url($url);
        $retUrl = '';

        if(isset($urlAr['host'])) // got the host and return
        {
            $retUrl.=$urlAr['host'];
        }

        if(isset($urlAr['path']))
        {
            $retUrl.= $urlAr['path'];
        }

        if(isset($urlAr['port'])) // an optional setting
        {
            $retUrl .= ":".$urlAr['port'];
        }

        return $retUrl;
    }

}