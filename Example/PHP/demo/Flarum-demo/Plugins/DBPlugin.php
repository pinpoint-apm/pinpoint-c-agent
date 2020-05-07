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

class DBPlugin extends Candy
{

///@hook:Illuminate\Database\Connection::__construct
///@hook:Illuminate\Database\Connection::select
///@hook:Illuminate\Database\Connection::selectFromWriteConnection
///@hook:Illuminate\Database\Connection::selectOne
///@hook:Illuminate\Database\Connection::cursor
///@hook:Illuminate\Database\Connection::insert
///@hook:Illuminate\Database\Connection::update
///@hook:Illuminate\Database\Connection::delete
    public function onBefore(){
	if(strpos($this->apId, "Connection::__construct")){
		$this->who->_pinpoint_intern_host=$this->args[3]['host'];
		return;
	}
        pinpoint_add_clue("stp",MYSQL);
	pinpoint_add_clues(PHP_ARGS,print_r($this->args[0],true));
	pinpoint_add_clue('dst',$this->who->_pinpoint_intern_host);
    }
///@hook:Illuminate\Database\Connectors\MySqlConnector::connect
    public function onEnd(&$ret){
        //pinpoint_add_clue("stp",MYSQL);
	//pinpoint_add_clue('dst',$this->args[0]["host"]);
    }

    public function onException($e){
        pinpoint_add_clue("EXP",$e->getMessage());
    }
}
