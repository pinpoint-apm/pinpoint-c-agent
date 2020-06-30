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

namespace Plugins;
require_once "PluginsDefines.php";

abstract class Candy
{
    protected $apId;
    protected $who;
    protected $args;
    protected $ret=null;

    public function __construct($apId,$who,&$args)
    {
        /// todo start_this_aspect_trace
        $this->apId = $apId;
        $this->who =  $who;
        $this->args = &$args;

        pinpoint_start_trace();
        pinpoint_add_clue("name",$apId);
    }

    public function __destruct()
    {
        pinpoint_end_trace();
    }

    abstract function onBefore();

    abstract function onEnd(&$ret);

    abstract function onException($e);
}
