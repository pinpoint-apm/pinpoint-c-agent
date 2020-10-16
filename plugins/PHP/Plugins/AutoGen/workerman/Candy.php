<?php
/**
 * Created by PhpStorm.
 * User: eeliu
 * Date: 10/15/20
 * Time: 11:31 AM
 */

namespace Plugins\AutoGen\workerman;

abstract class Candy
{
    protected $apId;
    protected $who;
    protected $args;
    protected $pinpoint_id;
    protected $ret=null;

    public function __construct($apId,$who,&...$args)
    {
        $parent_id = Context::getInstance()->getId();
        $this->apId = $apId;
        $this->who =  $who;
        $this->args = &$args;
        $newid = pinpoint_start_trace($parent_id);
        // store current id into candy
        $this->pinpoint_id = $newid;
        // update global id
        Context::getInstance()->setId($newid);
        pinpoint_add_clue(INTERCEPTER_NAME,$apId,$this->pinpoint_id);
        echo "$apId start -> $parent_id $newid \n";
    }

    public function __destruct()
    {

    }

    abstract function onBefore();

    abstract function onException($e);

    function onEnd(&$ret)
    {
        // update pinpoint_id
        $id = pinpoint_end_trace($this->pinpoint_id);
        Context::getInstance()->setId($id);
        echo  $this->apId." end ".$this->pinpoint_id."\n";
    }

}