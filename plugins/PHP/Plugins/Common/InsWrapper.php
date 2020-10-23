<?php
/**
 * Created by PhpStorm.
 * User: eeliu
 * Date: 10/10/20
 * Time: 5:41 PM
 */

namespace Plugins\Common;


class InsWrapper
{
    protected $origin;
    public function __construct($inc)
    {
        $this->origin = $inc;
    }

    public function __call($name, $arguments)
    {
        return call_user_func_array([&$this->origin,$name],$arguments);
    }

    public static function __callStatic($name, $arguments)
    {
        // TODO: Implement __callStatic() method.
    }
}