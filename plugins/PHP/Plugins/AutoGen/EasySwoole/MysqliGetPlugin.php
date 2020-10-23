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
        pinpoint_add_clue(PP_SERVER_TYPE,PP_MYSQL,$this->id);
        pinpoint_add_clue(PP_DESTINATION,"$host,$port",$this->id);

    }

    function onEnd(&$ret)
    {
        pinpoint_add_clue(PP_SQL_FORMAT,$this->who->getLastQuery(),$this->id);
    }

    function onException($e)
    {

    }
}