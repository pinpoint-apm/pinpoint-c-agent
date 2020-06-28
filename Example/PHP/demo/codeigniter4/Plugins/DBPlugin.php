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
use Plugins\PDOStatement;

///@hook:CodeIgniter\Database\MySQLi\Connection::execute
class DBPlugin extends Candy
{

    public function onBefore()
    {
        if(strpos($this->apId, "execute"))
        {
            pinpoint_add_clue("stp",MYSQL);
            pinpoint_add_clues(PHP_ARGS, sprintf("sql:%s",$this->args[0]));
            pinpoint_add_clue("dst",getenv('database.default.hostname'));
            
            return;
        }
    }

    public function onEnd(&$ret)
    {
        if(strpos($this->apId, "prepare")){
            $origin_state = $ret;
            $ret = new PDOStatement($origin_state);
            return;
        }
    }

    public function onException($e)
    {

    }
}