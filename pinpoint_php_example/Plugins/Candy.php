<?php
/**
 * User: eeliu
 * Date: 1/4/19
 * Time: 3:23 PM
 */

namespace Plugins;
require_once "PluginsDefines.php";

abstract class Candy
{
    protected $apId;
    protected $who;
    protected $args;
    protected $ret=null;

    public function __construct($apId,$who,&...$args)
    {
        /// todo start_this_aspect_trace
        $this->apId = $apId;
        $this->who =  $who;
        $this->args = &$args;

        echo $apId." start <br> ";
        pinpoint_start_trace();
        pinpoint_add_clue("name",$apId);
    }

    public function __destruct()
    {
        echo $this->apId." end <br>";
        pinpoint_end_trace();
    }

    abstract function onBefore();

    abstract function onEnd(&$ret);

    abstract function onException($e);
}
