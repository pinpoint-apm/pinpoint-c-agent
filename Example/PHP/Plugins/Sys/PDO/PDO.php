<?php


namespace Plugins\Sys\PDO;

class PDO extends \PDO
{
    public $dsn;

    public function __construct ($dsn, $username, $passwd, $options) {
        $this->dsn = $dsn;
        parent::__construct($dsn, $username, $passwd, $options);
    }

    public function query($statement, $mode = \PDO::ATTR_DEFAULT_FETCH_MODE, $arg3 = null, array $ctorargs = array())
    {
        $var = new PDOGlueStatement("PDO::query",$this,$statement,$mode,$arg3,$ctorargs);
        try{
            $var->onBefore();
            $ret = parent::query($statement,$statement,$mode,$arg3,$ctorargs);
            $var->onEnd($ret);
            return $ret;
        }catch (\Exception $e){
            $var->onException($e);
            throw new \Exception($e);
        }

    }

    public function exec($statement)
    {
        $var = new PDOExec("PDO::exec",$this,$statement);
        try{
            $var->onBefore();
            $ret = parent::exec($statement);
            $var->onEnd($ret);
            return $ret;
        }catch (\Exception $e){
            $var->onException($e);
            throw new \Exception($e);
        }
    }


    public function prepare($statement,  $driver_options = array())
    {
        $var = new PDOGlueStatement("PDO::prepare",$this,$statement,$driver_options);
        try{
            $var->onBefore();
            $ret = parent::prepare($statement,$driver_options);
            $var->onEnd($ret);
            return $ret;
        }catch (\Exception $e){
            $var->onException($e);
            throw new \Exception($e);
        }
    }

}