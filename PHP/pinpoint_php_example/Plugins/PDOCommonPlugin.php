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

///@hook:app\TestPDO::\PDO::query app\TestPDO::\PDO::prepare
///@hook:app\TestPDO::\PDO::__construct
class PDOCommonPlugin extends Candy
{
    function onBefore()
    {
        pinpoint_add_clue("stp",MYSQL);
        if(strpos($this->apId, "PDO::__construct")){
            $this->who->url = $this->get_host($this->args[0][0]);
            pinpoint_add_clues(PHP_ARGS, sprintf("dsn:%s,username:%s,password:%s",$this->args[0][0], $this->args[0][1], $this->args[0][2]));
        }else{
            pinpoint_add_clues(PHP_ARGS, sprintf("sql:%s",$this->args[0][0]));
        }
        pinpoint_add_clue("dst",$this->who->url);
    }
    function onEnd(&$ret)
    {
        echo "ret:";
        var_dump($ret);
    }

    function onException($e)
    {
        pinpoint_add_clue("EXP",$e->getMessage());
    }

    function get_host($dsn){
        $dsn = explode(':', $dsn);
        $temp = explode(';', $dsn[1]);
        $host = [];
        foreach ($temp as $h){
            $h = explode('=', $h);
            $host[] = $h[1];
        }
        $host = implode(":", $host);
        return $host;
    }
}
