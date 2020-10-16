<?php
namespace Plugins\Framework\Swoole;

$context = new \Co\Context;

class IDContext
{

    public static function get()
    {
        return \Co::getContext()["id"];
    }
    public static function set($id)
    {
        \Co::getContext()["id"] = $id;
    }
}