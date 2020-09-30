<?php


namespace Plugins\Sys\PDO;

use Plugins\Common\Candy;
use Plugins\Sys\PDO\ProfilerPDOStatement;
class PDOGlueStatement extends Candy

{
    function onBefore()
    {
        // todo stp, should follow the dsn
        $dbInfo = $this->parseDb($this->who->dsn);
        pinpoint_add_clue(SERVER_TYPE,MYSQL);
        pinpoint_add_clues(PHP_ARGS, sprintf("sql:%s",$this->args[0]));
        pinpoint_add_clue(DESTINATION,$dbInfo['host']);
    }
    function onEnd(&$ret)
    {
        $origin = $ret;
        $ret = new ProfilerPDOStatement($origin);
    }

    function onException($e)
    {
        pinpoint_add_clue(ADD_EXCEPTION,$e->getMessage());
    }

    function parseDb($dsn){
        $db_url =   parse_url($dsn);
        parse_str(str_replace(';','&',$db_url['path']),$dbInfo);
        $dbInfo['scheme']= $db_url['scheme'];
        return $dbInfo;
    }
}