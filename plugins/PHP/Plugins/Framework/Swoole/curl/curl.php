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

/*
static $chArr= [];

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
        CurlUtil::curlPinpointHeader($ch,$value);
    }elseif ($option == CURLOPT_URL ){
        global $curlChAr;
        $curlChAr[strval($ch)] = ['url'=>$value];
    }

    return \curl_setopt($ch, $option, $value);
}

function curl_setopt_array($ch, array $options)
{

    $url = $options[CURLOPT_URL];
    CurlUtil::appendPinpointHeader($url,$options[CURLOPT_HTTPHEADER]);
    $ret =  \curl_setopt_array($ch,$options);
    return $ret;

}
function curl_exec($ch)
{
    $args = \pinpoint_get_func_ref_args();
    $commPlugins_curl_exec_var = new CurlExecPlugin('curl_exec', null, $args);
    try {
        $commPlugins_curl_exec_var->onBefore();
        $commPlugins_curl_exec_ret = call_user_func_array('curl_exec', $args);
        $commPlugins_curl_exec_var->onEnd($commPlugins_curl_exec_ret);
        return $commPlugins_curl_exec_ret;
    } catch (\Exception $e) {
        $commPlugins_curl_exec_var->onException($e);
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
*/

static $curlChAr= [];
const CH_URL="url";
const CH_HEADRR="header";
function curl_init($url = null)
{
    $ch = \curl_init($url);
    global $curlChAr;
    if($url){
        $curlChAr[strval($ch)] = [CH_URL=>$url,CH_HEADRR=>[]];
    }else{
        $curlChAr[strval($ch)] = [CH_URL=>"",
            CH_HEADRR=>[]];
    }
    return $ch;
}

function curl_setopt($ch, $option, $value)
{
    if($option == CURLOPT_HTTPHEADER)
    {
        global $curlChAr;
        $curlChAr[strval($ch)][CH_HEADRR] = $value;

    }elseif ($option == CURLOPT_URL ){
        global $curlChAr;
        $curlChAr[strval($ch)][CH_URL]=$value;
    }

    return \curl_setopt($ch, $option, $value);
}

function curl_setopt_array($ch, array $options)
{
    global $curlChAr;
    if(isset($options[CURLOPT_URL]))
    {

        $curlChAr[strval($ch)][CH_URL] = $options[CURLOPT_URL];
    }

    if(isset($options[CURLOPT_HTTPHEADER]))
    {

        $curlChAr[strval($ch)][CH_HEADRR] = $options[CURLOPT_HTTPHEADER];
    }

    return \curl_setopt_array($ch,$options);
}


function set_pinpoint_header($ch,$url,$userHeader)
{
    $ppHeader = CurlUtil::getPinpointHeader();
    $header= array_merge($userHeader,$ppHeader);
    \curl_setopt($ch,CURLOPT_HTTPHEADER,$header);
}


function curl_exec($ch)
{
    global $curlChAr;
    $chAr= $curlChAr[strval($ch)];
    $plugin = new CurlExecPlugin('curl_exec', null, $ch);
    try {
        $plugin->onBefore();
        set_pinpoint_header($ch,$chAr[CH_URL],$chAr[CH_HEADRR]);
        $ret= \curl_exec($ch);
        $plugin->onEnd($chAr[CH_URL]);
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
