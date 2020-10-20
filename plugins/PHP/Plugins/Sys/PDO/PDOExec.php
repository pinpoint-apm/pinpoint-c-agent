<?php

namespace Plugins\Sys\PDO;

use Plugins\Common\Candy;

class PDOExec extends Candy
{

    function onBefore()
    {
        pinpoint_add_clues(PP_PHP_ARGS, sprintf("sql:%s",$this->args[0][0]));
    }

    function onEnd(&$ret)
    {
        pinpoint_add_clues(PP_PHP_RETURN,"$ret");
    }

    function onException($e)
    {
        // TODO: Implement onException() method.
    }
}