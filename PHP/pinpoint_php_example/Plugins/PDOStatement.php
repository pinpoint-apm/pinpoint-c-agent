<?php
#-------------------------------------------------------------------------------
# Copyright 2019 NAVER Corp
# 
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License.  You may obtain a copy
# of the License at
# 
#   http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations under
# the License.
#-------------------------------------------------------------------------------

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
        pinpoint_add_clue("stp",PHP_METHOD);
        pinpoint_add_clues(PHP_ARGS,print_r($this->args,true));
        #####################################
    }
    protected function onEnd(&$ret)
    {
        #####################################
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