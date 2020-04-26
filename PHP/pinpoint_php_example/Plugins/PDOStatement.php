<?php


namespace Plugins;
use Plugins\InstancePlugins;

class PDOStatement extends InstancePlugins
{
    public function __construct(&$instance)
    {
        parent::__construct($instance);
        $this->_name_list []= "execute"; // hook PDOStatement::execute
        $this->_name_list []= "bindParam"; // hook PDOStatement::execute
    }

    protected function onBefore()
    {
        parent::onBefore();
        #####################################
        echo "--------------------------before";
        pinpoint_add_clue("stp",PHP_METHOD);
        pinpoint_add_clues(PHP_ARGS,print_r($this->args,true));
        #####################################
    }
    protected function onEnd(&$ret)
    {
        #####################################

        echo "--------------------------end";
        pinpoint_add_clues(PHP_RETURN,print_r($ret,true));
        #####################################
        parent::onEnd($ret);
    }

    protected function onException($e)
    {
        // do nothing
    }

    public function bindParam ($parameter, &$variable, $data_type = PDO::PARAM_STR, $length = null, $driver_options = null)
    {
        $args = \pinpoint_get_func_ref_args();
        return $this->__call(__FUNCTION__,$args);
    }

    public function bindColumn ($column, &$param, $type = null, $maxlen = null, $driverdata = null)
    {
        $args = \pinpoint_get_func_ref_args();
        return $this->__call(__FUNCTION__,$args);
    }



}