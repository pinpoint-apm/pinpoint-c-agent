<?php
/*
 * User: eeliu
 * Date: 11/5/20
 * Time: 3:09 PM
 */

namespace Plugins\Sys\Memcached;
use Plugins\Common\Candy;

class MemAddServersPlugin extends Candy
{

    function onBefore()
    {
        $servers = $this->args[0];
        $url ='';
        foreach ($servers as $server)
        {
            $url.= sprintf("%s:%d ",$server[0],$server[1]);
        }
        pinpoint_add_clue(PP_SERVER_TYPE,PP_MEMCACHED);
        pinpoint_add_clue(PP_DESTINATION,$url);
    }

    function onEnd(&$ret)
    {
        // TODO: Implement onEnd() method.
    }
}