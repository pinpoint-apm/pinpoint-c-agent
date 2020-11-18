<?php

namespace Plugins\Sys\mysqli;


use Plugins\Common\Candy;

class MysqliPreparePlugin extends Candy
{
    function onBefore()
    {
        $myqli = $this->who;
        pinpoint_add_clue(PP_SERVER_TYPE,PP_MYSQL);
        pinpoint_add_clue(PP_SQL_FORMAT, $this->args[0]);
        pinpoint_add_clue(PP_DESTINATION,$myqli->host_info);
    }

    function onEnd(&$ret)
    {
        $origin = $ret;
        $ret = new ProfilerMysqli_Stmt($origin);
    }
}