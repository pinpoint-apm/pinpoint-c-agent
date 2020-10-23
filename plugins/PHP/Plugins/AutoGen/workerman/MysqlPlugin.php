<?php

namespace Plugins\AutoGen\workerman;
use Workerman\MySQL\Connection as MysqlConnection;
/**
 * Class MysqlPlugin
 * @hook: Workerman\MySQL\Connection::execute
 */
class MysqlPlugin extends Candy
{

    function onBefore()
    {
        $query = $this->args[0];
//        $parameters = $this->args[1];
        pinpoint_add_clue(PP_SERVER_TYPE,PP_MYSQL,$this->pinpoint_id);
        assert($this->who instanceof MysqlConnection);
        $setting = $this->getSetting($this->who);
        $dst = $setting['host'].":".$setting['port'];
        pinpoint_add_clue(PP_DESTINATION,$dst,$this->pinpoint_id);
        pinpoint_add_clue(PP_SQL_FORMAT,$query,$this->pinpoint_id);

    }

    function onEnd(&$ret)
    {


        parent::onEnd($ret);
    }

    function onException($e)
    {

    }

    function getSetting($conn)
    {
        $getSetting = function (){
            return $this->settings;
        };

        return $getSetting->call($conn);
    }

}