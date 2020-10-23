<?php
namespace Plugins\Framework\Swoole\curl;
use Plugins\Sys\curl\CurlUtil;

static $chArr= [];

function curl_init($url = null)
{
    $ch = \curl_init($url);
    global $chArr;
    if($url){
        $chArr[strval($ch)] = ['url'=>$url];
    }else{
        $chArr[strval($ch)] = [];
    }
    return $ch;
}


function curl_setopt($ch, $option, $value)
{
    if($option == CURLOPT_HTTPHEADER)
    {
        CurlUtil::curlPinpointHeader($ch,$value);
    }elseif ($option == CURLOPT_URL ){
        global $chArr;
        $chArr[strval($ch)] = ['url'=>$value];
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
    $commPlugins_curl_exec_var = new NextSpanPlugin('curl_exec', null, $args);
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
    global $chArr;
    unset($chArr[strval($ch)] );
    \curl_close($ch);
}

function curl_reset($ch)
{
    global $ch_arr;
    $ch_arr[strval($ch)] = [];
    \curl_reset($ch);
}
