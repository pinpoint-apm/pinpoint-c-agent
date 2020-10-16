<?php
/**
 * Created by PhpStorm.
 * User: eeliu
 * Date: 10/15/20
 * Time: 2:37 PM
 */

namespace Plugins\AutoGen\workerman;


class Context
{
    private static $context=null;
    private $id;
    public static function getInstance()
    {
        if(self::$context == null){
            self::$context = new Context();
        }
        return self::$context;
    }

    public function setId($id)
    {
        assert($id!=0);
        $this->id = $id;
    }
    public function getId()
    {
        return $this->id;
    }
}