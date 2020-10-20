<?php
/**
 * Created by PhpStorm.
 * User: eeliu
 * Date: 10/10/20
 * Time: 6:33 PM
 */

namespace Plugins\AutoGen\EasySwoole;

use Plugins\Framework\Swoole\IDContext;

/**
 * @hook: EasySwoole\Http\Response::
 */
class ResponsePlugin
{
    public function __construct($apId,$who,&...$args)
    {
        $id = IDContext::get();
        $code = $args[0];
        pinpoint_add_clues(HTTP_STATUS_CODE,$code,$id,PP_ROOT_LOC);
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