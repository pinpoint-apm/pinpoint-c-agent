<?php
namespace Plugins\Sys\Memcached;

use Plugins\Common\Candy;

class MemGetPlugin extends Candy
{

    function onBefore()
    {
        $key =  $this->args[0];
        pinpoint_add_clue(PP_SERVER_TYPE,PP_MEMCACHED);
        pinpoint_add_clues(PP_PHP_ARGS,"$key");
    }

    function onEnd(&$ret)
    {

    }
}