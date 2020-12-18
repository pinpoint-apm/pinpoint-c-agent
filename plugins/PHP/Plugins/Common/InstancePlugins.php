<?php
/******************************************************************************
 * Copyright 2020 NAVER Corp.                                                 *
 *                                                                            *
 * Licensed under the Apache License, Version 2.0 (the "License");            *
 * you may not use this file except in compliance with the License.           *
 * You may obtain a copy of the License at                                    *
 *                                                                            *
 *     http://www.apache.org/licenses/LICENSE-2.0                             *
 *                                                                            *
 * Unless required by applicable law or agreed to in writing, software        *
 * distributed under the License is distributed on an "AS IS" BASIS,          *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   *
 * See the License for the specific language governing permissions and        *
 * limitations under the License.                                             *
 ******************************************************************************/



namespace Plugins\Common;


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
        pinpoint_add_clue(PP_INTERCEPTOR_NAME,$this->_name);
    }
    protected function onEnd(&$ret)
    {
        pinpoint_end_trace();
    }

    protected function onException($e)
    {
        pinpoint_add_clue(PP_ADD_EXCEPTION,$e->getMessage());
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

    public function __toString():string
    {
        return $this->_instance->__toString();
    }

    public function __invoke()
    {
        return $this->_instance->__invoke();
    }

    public function __clone()
    {
        $this->_instance = clone $this->_instance;
    }


}