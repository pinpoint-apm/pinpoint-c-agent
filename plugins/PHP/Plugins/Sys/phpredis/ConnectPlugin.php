<?php
/*
 * User: eeliu
 * Date: 11/5/20
 * Time: 4:18 PM
 */

namespace Plugins\Sys\phpredis;


use Plugins\Common\Candy;

class ConnectPlugin extends  Candy
{

    function onBefore()
    {
        $host = $this->args[0];
        $port = $this->args[1];
        pinpoint_add_clue(PP_SERVER_TYPE,PP_REDIS);
        pinpoint_add_clue(PP_DESTINATION,"$host:$port");
    }

    function onEnd(&$ret)
    {
        // TODO: Implement onEnd() method.
    }
}