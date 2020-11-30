<?php

namespace Plugins\Sys\mysqli;

class ProfilerMysqliResult
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

}