<?php

namespace Plugins\AutoGen\EasySwoole;
use Plugins\Framework\Swoole\Candy;

/**
 * Class MysqliSelectDbPlugin
 * @hook: EasySwoole\Mysqli\Mysqli::selectDb
 */
class MysqliSelectDbPlugin extends Candy
{

    function onBefore()
    {
        pinpoint_add_clue(PP_PHP_ARGS,$this->args[0],$this->id);
    }

    function onEnd(&$ret)
    {

    }

    function onException($e)
    {

    }
}