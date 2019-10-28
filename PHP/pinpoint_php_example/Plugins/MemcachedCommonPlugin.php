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
use Plugins\Candy;

///@hook:app\TestMemcached::\memcached::addServer app\TestMemcached::\memcached::add app\TestMemcached::\memcached::get app\TestMemcached::\memcached::replace app\TestMemcached::\memcached::delete
class MemcachedCommonPlugin extends Candy
{
    function onBefore()
    {
        pinpoint_add_clue("stp",MEMCACHED);
        if(strpos($this->apId, "memcached::addServer")){
            $this->who->url = sprintf("%s:%s",$this->args[0][0],$this->args[0][1]);
            pinpoint_add_clues(PHP_ARGS, sprintf("host:%s,port:%s",$this->args[0][0], $this->args[0][1]));
        }elseif(count($this->args[0])==2 && !strpos($this->apId, "memcached::addServer")){
            pinpoint_add_clues(PHP_ARGS, sprintf("key:%s,value:%s",$this->args[0][0], $this->args[0][1]));
        }elseif(count($this->args[0])==1){
            pinpoint_add_clues(PHP_ARGS, sprintf("key:%s",$this->args[0][0]));
        }
        pinpoint_add_clue("dst", $this->who->url);
    }
    function onEnd(&$ret)
    {
        echo "ret:";
        pinpoint_add_clues(PHP_RETURN, print_r($ret,true));
    }

    function onException($e)
    {
        pinpoint_add_clue("EXP",$e->getMessage());
    }
}
