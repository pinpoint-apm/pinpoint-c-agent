<?php
/*
 * User: eeliu
 * Date: 11/5/20
 * Time: 4:32 PM
 */

namespace Plugins\Sys\phpredis;


use Plugins\Common\Candy;

class SetPlugin extends Candy
{

    function onBefore()
    {
        $key = $this->args[0];
        $value = $this->args[1];
        pinpoint_add_clue(PP_SERVER_TYPE,PP_REDIS);
        pinpoint_add_clues(PP_PHP_ARGS,"$key:$value");
    }

    function onEnd(&$ret)
    {
        // TODO: Implement onEnd() method.
    }
}