<?php
/**
 * User: eeliu
 * Date: 3/29/19
 * Time: 3:40 PM
 */

namespace Plugins;
use Plugins\Candy;

///@hook:app\DBcontrol::\PDO::query
///@hook:app\DBcontrol::\PDO::exec
///@hook:app\DBcontrol::\PDO::errorCode
class PDOCommonPlugin extends Candy
{
    function onBefore()
    {
        if($this->apId == 'app\PDO::query'){

//            $this->who
        }
    }
    ///@hook:app\DBcontrol::getData1 app\DBcontrol::\array_push
    function onEnd(&$ret)
    {
        // TODO: Implement onEnd() method.
    }

    function onException($e)
    {
        // TODO: Implement onException() method.
    }
}