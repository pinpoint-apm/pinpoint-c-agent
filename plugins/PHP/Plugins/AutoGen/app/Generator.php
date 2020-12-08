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


namespace Plugins\AutoGen;
use Plugins\Common\InstancePlugins;

class Generator extends InstancePlugins implements \Iterator
{
    public function __construct(&$instance)
    {
        parent::__construct($instance);
        $this->_name_list []= "next"; // hook Generator::next
    }

    protected function onBefore()
    {
        parent::onBefore();
        #####################################
//        echo "--------------------------before";
        pinpoint_add_clue("stp",PP_PHP_METHOD);
        pinpoint_add_clues(PP_PHP_ARGS,"--placeholder---");
        #####################################
    }
    protected function onEnd(&$ret)
    {
        #####################################

//        echo "--------------------------end";
        pinpoint_add_clues(PP_PHP_RETURN,"--placeholder---");
        #####################################
        parent::onEnd($ret);
    }

    protected function onException($e)
    {
        // do nothing
    }

    public function current()
    {
        return $this->_instance->current();
    }

    public function next()
    {
        return $this->__call(__FUNCTION__, []);
    }

    public function key()
    {
        return $this->_instance->key();
    }

    public function valid()
    {
        return $this->_instance->valid();
    }

    public function rewind()
    {
        return $this->_instance->rewind();
    }
}