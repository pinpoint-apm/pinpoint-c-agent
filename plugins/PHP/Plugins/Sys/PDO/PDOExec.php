<?php
/**
 * Created by PhpStorm.
 * User: eeliu
 * Date: 8/17/20
 * Time: 3:57 PM
 */

namespace Plugins\Sys\PDO;

use Plugins\Common\Candy;

class PDOExec extends Candy
{

    function onBefore()
    {
        pinpoint_add_clues(PHP_ARGS, sprintf("sql:%s",$this->args[0][0]));
    }

    function onEnd(&$ret)
    {
        pinpoint_add_clues(PHP_RETURN,"$ret");
    }

    function onException($e)
    {
        // TODO: Implement onException() method.
    }
}