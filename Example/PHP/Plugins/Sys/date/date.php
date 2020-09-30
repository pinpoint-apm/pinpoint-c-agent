<?php
namespace Plugins\Sys\date;
use Plugins\Common\CommonPlugin;

function date () {
    $args = \pinpoint_get_func_ref_args();

    $plugins = new CommonPlugin('date', null, $args);
    try {
        $plugins->onBefore();
        $ret = call_user_func_array('date', $args);
        $plugins->onEnd($ret);
        return $ret;
    } catch (\Exception $e) {
        $plugins->onException($e);
        throw $e;
    }

}