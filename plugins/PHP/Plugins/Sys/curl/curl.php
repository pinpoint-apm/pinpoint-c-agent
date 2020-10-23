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

static $curlChAr= [];

function curl_init($url = null)
{
    $ch = \curl_init($url);
    global $curlChAr;
    if($url){
        $curlChAr[strval($ch)] = ['url'=>$url];

    }else{
        $curlChAr[strval($ch)] = [];
    }
    return $ch;
}


function curl_setopt($ch, $option, $value)
{
    if($option == CURLOPT_HTTPHEADER)
    {
        CurlUtil::appendPinpointHeader($value);
    }elseif ($option == CURLOPT_URL ){
        global $curlChAr;
        $curlChAr[strval($ch)] = ['url'=>$value];
    }

    return \curl_setopt($ch, $option, $value);
}

function curl_setopt_array($ch, array $options)
{
    $args = \pinpoint_get_func_ref_args();
    $plugin = new NextSpanPlugin('curl_setopt_array', null, $args);
    try {
        $plugin->onBefore();
        $ret = call_user_func_array('curl_setopt_array', $args);
        $plugin->onEnd($ret);
        return $ret;
    } catch (\Exception $e) {
        $plugin->onException($e);
        throw $e;
    }
}

function curl_exec($ch)
{
    $args = \pinpoint_get_func_ref_args();
    $plugin = new NextSpanPlugin('curl_exec', null, $args);
    try {
        $plugin->onBefore();
        $ret = call_user_func_array('curl_exec', $args);
        $plugin->onEnd($ret);
        return $ret;
    } catch (\Exception $e) {
        $plugin->onException($e);
        throw $e;
    }
}

function curl_close($ch)
{
    global $curlChAr;
    unset($curlChAr[strval($ch)] );
    \curl_close($ch);
}

function curl_reset($ch)
{
    global $ch_arr;
    $ch_arr[strval($ch)] = [];
    \curl_reset($ch);
}
