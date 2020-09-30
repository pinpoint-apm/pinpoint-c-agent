<?php
namespace Plugins\Sys\curl;

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
//    $args = \pinpoint_get_func_ref_args();
//    $commPlugins_curl_init_var = new commPlugins('curl_init', null, $args);
//    try {
//        $commPlugins_curl_init_var->onBefore();
//        $commPlugins_curl_init_ret = call_user_func_array('curl_init', $args);
//        $commPlugins_curl_init_var->onEnd($commPlugins_curl_init_ret);
//        return $commPlugins_curl_init_ret;
//    } catch (\Exception $e) {
//        $commPlugins_curl_init_var->onException($e);
//        throw $e;
//    }
}
//function curl_copy_handle($ch)
//{
//    $args = \pinpoint_get_func_ref_args();
//    $commPlugins_curl_copy_handle_var = new commPlugins('curl_copy_handle', null, $args);
//    try {
//        $commPlugins_curl_copy_handle_var->onBefore();
//        $commPlugins_curl_copy_handle_ret = call_user_func_array('curl_copy_handle', $args);
//        $commPlugins_curl_copy_handle_var->onEnd($commPlugins_curl_copy_handle_ret);
//        return $commPlugins_curl_copy_handle_ret;
//    } catch (\Exception $e) {
//        $commPlugins_curl_copy_handle_var->onException($e);
//        throw $e;
//    }
//}
//function curl_version($version = null)
//{
//    $args = \pinpoint_get_func_ref_args();
//    $commPlugins_curl_version_var = new commPlugins('curl_version', null, $args);
//    try {
//        $commPlugins_curl_version_var->onBefore();
//        $commPlugins_curl_version_ret = call_user_func_array('curl_version', $args);
//        $commPlugins_curl_version_var->onEnd($commPlugins_curl_version_ret);
//        return $commPlugins_curl_version_ret;
//    } catch (\Exception $e) {
//        $commPlugins_curl_version_var->onException($e);
//        throw $e;
//    }
//}


function curl_setopt($ch, $option, $value)
{
    if($option == CURLOPT_HTTPHEADER)
    {
        CurlUtil::appendPinpointHeader($value);
    }elseif ($option == CURLOPT_URL ){
        CurlUtil::appendPinpointHeader($value);
    }

    return \curl_setopt($ch, $option, $value);


//    $args = \pinpoint_get_func_ref_args();
//    $commPlugins_curl_setopt_var = new NextSpanPlugin('curl_setopt', null, $args);
//    try {
//        $commPlugins_curl_setopt_var->onBefore();
//        $commPlugins_curl_setopt_ret = call_user_func_array('curl_setopt', $args);
//        $commPlugins_curl_setopt_var->onEnd($commPlugins_curl_setopt_ret);
//        return $commPlugins_curl_setopt_ret;
//    } catch (\Exception $e) {
//        $commPlugins_curl_setopt_var->onException($e);
//        throw $e;
//    }
}
function curl_setopt_array($ch, array $options)
{
    $args = \pinpoint_get_func_ref_args();
    $commPlugins_curl_setopt_array_var = new NextSpanPlugin('curl_setopt_array', null, $args);
    try {
        $commPlugins_curl_setopt_array_var->onBefore();
        $commPlugins_curl_setopt_array_ret = call_user_func_array('curl_setopt_array', $args);
        $commPlugins_curl_setopt_array_var->onEnd($commPlugins_curl_setopt_array_ret);
        return $commPlugins_curl_setopt_array_ret;
    } catch (\Exception $e) {
        $commPlugins_curl_setopt_array_var->onException($e);
        throw $e;
    }
}
function curl_exec($ch)
{

    // add curl_exec

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

function curl_multi_add_handle($mh, $ch)
{
    $args = \pinpoint_get_func_ref_args();
    $commPlugins_curl_multi_add_handle_var = new commPlugins('curl_multi_add_handle', null, $args);
    try {
        $commPlugins_curl_multi_add_handle_var->onBefore();
        $commPlugins_curl_multi_add_handle_ret = call_user_func_array('curl_multi_add_handle', $args);
        $commPlugins_curl_multi_add_handle_var->onEnd($commPlugins_curl_multi_add_handle_ret);
        return $commPlugins_curl_multi_add_handle_ret;
    } catch (\Exception $e) {
        $commPlugins_curl_multi_add_handle_var->onException($e);
        throw $e;
    }
}
function curl_multi_remove_handle($mh, $ch)
{
    // $args = \pinpoint_get_func_ref_args();
    // $commPlugins_curl_multi_remove_handle_var = new commPlugins('curl_multi_remove_handle', null, $args);
    // try {
    //     $commPlugins_curl_multi_remove_handle_var->onBefore();
    //     $commPlugins_curl_multi_remove_handle_ret = call_user_func_array('curl_multi_remove_handle', $args);
    //     $commPlugins_curl_multi_remove_handle_var->onEnd($commPlugins_curl_multi_remove_handle_ret);
    //     return $commPlugins_curl_multi_remove_handle_ret;
    // } catch (\Exception $e) {
    //     $commPlugins_curl_multi_remove_handle_var->onException($e);
    //     throw $e;
    // }
}

// function curl_multi_select($mh, $timeout = null)
// {
//     $args = \pinpoint_get_func_ref_args();
//     $commPlugins_curl_multi_select_var = new commPlugins('curl_multi_select', null, $args);
//     try {
//         $commPlugins_curl_multi_select_var->onBefore();
//         $commPlugins_curl_multi_select_ret = call_user_func_array('curl_multi_select', $args);
//         $commPlugins_curl_multi_select_var->onEnd($commPlugins_curl_multi_select_ret);
//         return $commPlugins_curl_multi_select_ret;
//     } catch (\Exception $e) {
//         $commPlugins_curl_multi_select_var->onException($e);
//         throw $e;
//     }
// }

function curl_multi_exec($mh, &$still_running = null)
{
    $args = \pinpoint_get_func_ref_args();
    $commPlugins_curl_multi_exec_var = new commPlugins('curl_multi_exec', null, $args);
    try {
        $commPlugins_curl_multi_exec_var->onBefore();
        $commPlugins_curl_multi_exec_ret = call_user_func_array('curl_multi_exec', $args);
        $commPlugins_curl_multi_exec_var->onEnd($commPlugins_curl_multi_exec_ret);
        return $commPlugins_curl_multi_exec_ret;
    } catch (\Exception $e) {
        $commPlugins_curl_multi_exec_var->onException($e);
        throw $e;
    }
}
function curl_multi_getcontent($ch)
{
    $args = \pinpoint_get_func_ref_args();
    $commPlugins_curl_multi_getcontent_var = new commPlugins('curl_multi_getcontent', null, $args);
    try {
        $commPlugins_curl_multi_getcontent_var->onBefore();
        $commPlugins_curl_multi_getcontent_ret = call_user_func_array('curl_multi_getcontent', $args);
        $commPlugins_curl_multi_getcontent_var->onEnd($commPlugins_curl_multi_getcontent_ret);
        return $commPlugins_curl_multi_getcontent_ret;
    } catch (\Exception $e) {
        $commPlugins_curl_multi_getcontent_var->onException($e);
        throw $e;
    }
}
function curl_multi_info_read($mh, &$msgs_in_queue = null)
{
    $args = \pinpoint_get_func_ref_args();
    $commPlugins_curl_multi_info_read_var = new commPlugins('curl_multi_info_read', null, $args);
    try {
        $commPlugins_curl_multi_info_read_var->onBefore();
        $commPlugins_curl_multi_info_read_ret = call_user_func_array('curl_multi_info_read', $args);
        $commPlugins_curl_multi_info_read_var->onEnd($commPlugins_curl_multi_info_read_ret);
        return $commPlugins_curl_multi_info_read_ret;
    } catch (\Exception $e) {
        $commPlugins_curl_multi_info_read_var->onException($e);
        throw $e;
    }
}
function curl_multi_close($mh)
{
    $args = \pinpoint_get_func_ref_args();
    $commPlugins_curl_multi_close_var = new commPlugins('curl_multi_close', null, $args);
    try {
        $commPlugins_curl_multi_close_var->onBefore();
        $commPlugins_curl_multi_close_ret = call_user_func_array('curl_multi_close', $args);
        $commPlugins_curl_multi_close_var->onEnd($commPlugins_curl_multi_close_ret);
        return $commPlugins_curl_multi_close_ret;
    } catch (\Exception $e) {
        $commPlugins_curl_multi_close_var->onException($e);
        throw $e;
    }
}
function curl_multi_errno($mh)
{
    $args = \pinpoint_get_func_ref_args();
    $commPlugins_curl_multi_errno_var = new commPlugins('curl_multi_errno', null, $args);
    try {
        $commPlugins_curl_multi_errno_var->onBefore();
        $commPlugins_curl_multi_errno_ret = call_user_func_array('curl_multi_errno', $args);
        $commPlugins_curl_multi_errno_var->onEnd($commPlugins_curl_multi_errno_ret);
        return $commPlugins_curl_multi_errno_ret;
    } catch (\Exception $e) {
        $commPlugins_curl_multi_errno_var->onException($e);
        throw $e;
    }
}
function curl_multi_setopt($sh, $option, $value)
{
    $args = \pinpoint_get_func_ref_args();
    $commPlugins_curl_multi_setopt_var = new commPlugins('curl_multi_setopt', null, $args);
    try {
        $commPlugins_curl_multi_setopt_var->onBefore();
        $commPlugins_curl_multi_setopt_ret = call_user_func_array('curl_multi_setopt', $args);
        $commPlugins_curl_multi_setopt_var->onEnd($commPlugins_curl_multi_setopt_ret);
        return $commPlugins_curl_multi_setopt_ret;
    } catch (\Exception $e) {
        $commPlugins_curl_multi_setopt_var->onException($e);
        throw $e;
    }
}
function curl_share_init()
{
    $args = \pinpoint_get_func_ref_args();
    $commPlugins_curl_share_init_var = new commPlugins('curl_share_init', null, $args);
    try {
        $commPlugins_curl_share_init_var->onBefore();
        $commPlugins_curl_share_init_ret = call_user_func_array('curl_share_init', $args);
        $commPlugins_curl_share_init_var->onEnd($commPlugins_curl_share_init_ret);
        return $commPlugins_curl_share_init_ret;
    } catch (\Exception $e) {
        $commPlugins_curl_share_init_var->onException($e);
        throw $e;
    }
}
function curl_share_close($sh)
{
    $args = \pinpoint_get_func_ref_args();
    $commPlugins_curl_share_close_var = new commPlugins('curl_share_close', null, $args);
    try {
        $commPlugins_curl_share_close_var->onBefore();
        $commPlugins_curl_share_close_ret = call_user_func_array('curl_share_close', $args);
        $commPlugins_curl_share_close_var->onEnd($commPlugins_curl_share_close_ret);
        return $commPlugins_curl_share_close_ret;
    } catch (\Exception $e) {
        $commPlugins_curl_share_close_var->onException($e);
        throw $e;
    }
}
function curl_share_setopt($sh, $option, $value)
{
    $args = \pinpoint_get_func_ref_args();
    $commPlugins_curl_share_setopt_var = new commPlugins('curl_share_setopt', null, $args);
    try {
        $commPlugins_curl_share_setopt_var->onBefore();
        $commPlugins_curl_share_setopt_ret = call_user_func_array('curl_share_setopt', $args);
        $commPlugins_curl_share_setopt_var->onEnd($commPlugins_curl_share_setopt_ret);
        return $commPlugins_curl_share_setopt_ret;
    } catch (\Exception $e) {
        $commPlugins_curl_share_setopt_var->onException($e);
        throw $e;
    }
}
function curl_share_errno($sh)
{
    $args = \pinpoint_get_func_ref_args();
    $commPlugins_curl_share_errno_var = new commPlugins('curl_share_errno', null, $args);
    try {
        $commPlugins_curl_share_errno_var->onBefore();
        $commPlugins_curl_share_errno_ret = call_user_func_array('curl_share_errno', $args);
        $commPlugins_curl_share_errno_var->onEnd($commPlugins_curl_share_errno_ret);
        return $commPlugins_curl_share_errno_ret;
    } catch (\Exception $e) {
        $commPlugins_curl_share_errno_var->onException($e);
        throw $e;
    }
}
function curl_file_create($filename, $mimetype = null, $postname = null)
{
    $args = \pinpoint_get_func_ref_args();
    $commPlugins_curl_file_create_var = new commPlugins('curl_file_create', null, $args);
    try {
        $commPlugins_curl_file_create_var->onBefore();
        $commPlugins_curl_file_create_ret = call_user_func_array('curl_file_create', $args);
        $commPlugins_curl_file_create_var->onEnd($commPlugins_curl_file_create_ret);
        return $commPlugins_curl_file_create_ret;
    } catch (\Exception $e) {
        $commPlugins_curl_file_create_var->onException($e);
        throw $e;
    }
}