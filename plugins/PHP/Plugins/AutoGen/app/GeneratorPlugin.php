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
use Plugins\Common\Candy;
use Plugins\AutoGen\Generator;

///@hook:app\TestGenerator::generator
class GeneratorPlugin extends Candy
{
    public function onBefore(){
        pinpoint_add_clue("stp",PP_PHP_METHOD);
        pinpoint_add_clues(PP_PHP_ARGS,print_r($this->args,true));
    }
    public function onEnd(&$ret){
        if (isset($ret)){
            $origin_state = $ret;
            $ret = new Generator($origin_state);
        }
    }
    public function onException($e){
        pinpoint_add_clue("EXP",$e->getMessage());
    }
}