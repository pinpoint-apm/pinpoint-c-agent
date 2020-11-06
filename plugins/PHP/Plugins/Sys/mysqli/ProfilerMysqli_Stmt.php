<?php
/*
 * User: eeliu
 * Date: 11/5/20
 * Time: 5:32 PM
 */

namespace Plugins\Sys\mysqli;


class ProfilerMysqli_Stmt
{
    protected $_instance;
    public function __construct(&$instance)
    {
        $this->_instance = &$instance;
    }

    public function __call($name, $arguments)
    {
        return call_user_func_array([&$this->_instance,$name],$arguments);
    }

    public function bind_param ($types, &$var1, &...$_)
    {
        $param = \pinpoint_get_func_ref_args();
        return call_user_func_array([$this->_instance,'bind_param'],$param);

    }

    public function bind_result (&$var1, &...$_)
    {
        $param = \pinpoint_get_func_ref_args();
        return call_user_func_array([$this->_instance,'bind_result'],$param);
    }

    public function execute()
    {
        $plugin = new StmtExecutePlugin("Stmt::execute",$this);
        try{
            $plugin->onBefore();
            $ret =  call_user_func_array([$this->_instance,'execute'],[]);
            $plugin->onEnd($ret);
            return $ret;

        }catch (\Exception $e)
        {
            $plugin->onException($e);
        }
    }


}