<?php


namespace Plugins;


class InstancePlugins
{
    protected $_instance;
    protected $_name_list=[];
    protected $args;
    protected $_name;
    protected $_classFullname;

    public function __construct(&$instance)
    {
        $this->_instance =&$instance;
        $this->_classFullname = get_class($instance);
    }

    protected function onBefore()
    {
        pinpoint_start_trace();
        pinpoint_add_clue("name",$this->_name);
    }
    protected function onEnd(&$ret)
    {
        pinpoint_end_trace();
    }

    protected function onException($e)
    {
        pinpoint_add_clue("EXP",$e->getMessage());
    }

    public function __call($name, $arguments)
    {
        if(!in_array($name ,$this->_name_list))
        {
            return call_user_func_array([$this->_instance,$name],$arguments);
        }else{ // should hooked
            try{
                $this->_name = $name;
                $this->args = $arguments;
                $this->onBefore();
                $ret = call_user_func_array([$this->_instance,$name],$arguments);
                $this->onEnd($ret);
                return $ret;
            }catch (\Exception $e){
                $this->onException($e);
                $ret = null;
                $this->onEnd($ret);
                throw $e;
            }
        }
    }

    public function __get($name)
    {
        return $this->_instance->$name;
    }

    public function __set($name, $value)
    {
        $this->_instance->$name = $value;
    }

    public function __isset($name)
    {
        return $this->_instance->__isset($name);
    }

    public  function __unset($name)
    {
        $this->_instance->__unset($name);
    }

    public function __sleep()
    {
       return $this->_instance->__sleep();
    }

    public function __wakeup()
    {
        $this->_instance->__wakeup();
    }

    public function __toString()
    {
        return $this->_instance->__toString();
    }

    public function __invoke()
    {
        return $this->_instance->__invoke();
    }

    public function __debugInfo()
    {
        return $this->_instance->__debugInfo();
    }

    public function __clone()
    {
        $this->_instance = clone $this->_instance;
    }


}