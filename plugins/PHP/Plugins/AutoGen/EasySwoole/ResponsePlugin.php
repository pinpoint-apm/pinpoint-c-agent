<?php

namespace Plugins\AutoGen\EasySwoole;

use Plugins\Framework\Swoole\IDContext;

/**
 * @hook: EasySwoole\Http\Message\Response::withStatus
 */
class ResponsePlugin
{
    public function __construct($apId,$who,&...$args)
    {
        $id = IDContext::get();
        $code = $args[0];
        pinpoint_set_context((string)PP_HTTP_STATUS_CODE,(string)$code,$id);
        if($code !== 200){
            pinpoint_add_clue(PP_ADD_EXCEPTION,"http response not 200. status=[$code]",$id,PP_ROOT_LOC);
        }
    }

    function onBefore()
    {
    }

    function onEnd(&$ret)
    {

    }

    function onException($e)
    {

    }
}