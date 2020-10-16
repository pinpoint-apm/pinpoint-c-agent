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

/**
 * User: eeliu
 * Date: 1/4/19
 * Time: 3:23 PM
 */

namespace Plugins\Framework\Swoole;

abstract class Candy
{
    protected $apId;
    protected $who;
    protected $args;
    protected $ret=null;
    protected $id;

    public function __construct($apId,$who,&...$args)
    {
        /// todo start_this_aspect_trace
        $this->apId = $apId;
        $this->who =  $who;
        $this->args = &$args;
        $this->id = pinpoint_start_trace(IDContext::get());
        IDContext::set($this->id);
        pinpoint_add_clue(INTERCEPTER_NAME,$apId,$this->id);
    }

    public function __destruct()
    {
        $id = pinpoint_end_trace($this->id);
        IDContext::set($id);
    }

    abstract function onBefore();

    abstract function onEnd(&$ret);

    abstract function onException($e);
}
