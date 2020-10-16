<?php

namespace Plugins\AutoGen\EasySwoole;
use Plugins\Framework\Swoole\Candy;

/**
 * Class MysqliSelectDbPlugin
 * @hook: EasySwoole\Mysqli\Mysqli::get
 * @hook: EasySwoole\Mysqli\Mysqli::insert
 * @hook: EasySwoole\Mysqli\Mysqli::update
 */
class MysqliGetPlugin extends Candy
{
    function onBefore()
    {
        $ins_mysqli = $this->who;
        $config = $ins_mysqli->getConfig();
        $host = $config->getHost();
        $port = $config->getPort();
        pinpoint_add_clue(SERVER_TYPE,MYSQL,$this->id);
        pinpoint_add_clue(DESTINATION,"$host,$port",$this->id);

    }

    function onEnd(&$ret)
    {
        pinpoint_add_clue(SQL_FORMAT,$this->who->getLastQuery(),$this->id);
    }

    function onException($e)
    {

    }
}