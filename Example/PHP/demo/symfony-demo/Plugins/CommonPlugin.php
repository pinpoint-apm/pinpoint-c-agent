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
 * The the functions you cared!
 * Example App\Controller\LuckyController::number
 * https://symfony.com/doc/current/page_creation.html#creating-a-page-route-and-controller
 */
namespace Plugins;
use Plugins\Candy;



///@hook:App\Controller\LuckyController::number
class CommonPlugin extends Candy
{
    public function onBefore(){
        pinpoint_add_clue("stp",PHP_METHOD);
        pinpoint_add_clues(PHP_ARGS,print_r($this->args,true));
    }

    public function onEnd(&$ret){
        pinpoint_add_clues(PHP_RETURN,print_r($this->ret,true));
    }

    public function onException($e){
        pinpoint_add_clue("EXP",$e->getMessage());
    }
}