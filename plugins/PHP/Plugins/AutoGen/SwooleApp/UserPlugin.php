<?php
/******************************************************************************
 * Copyright 2020 NAVER Corp.                                                 *
 *                                                                            *
 * Licensed under the Apache License, Version 2.0 (the "License");            *
 * you may not use this file except in compliance with the License.           *
 * You may obtain a copy of the License at                                    *
 *                                                                            *
 *     http://www.apache.org/licenses/LICENSE-2.0                             *
 *                                                                            *
 * Unless required by applicable law or agreed to in writing, software        *
 * distributed under the License is distributed on an "AS IS" BASIS,          *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   *
 * See the License for the specific language governing permissions and        *
 * limitations under the License.                                             *
 ******************************************************************************/



namespace Plugins\AutoGen\SwooleApp;
use Plugins\Framework\Swoole\Candy;

/**
 * @hook:app\HttpServer::doSomething app\HttpServer::doSomething1 app\HttpServer::doSomething2
 * @hook:app\HttpServer::doAny
 */
class UserPlugin extends Candy
{
    public function onBefore(){
        pinpoint_add_clue(PP_SERVER_TYPE,PP_PHP_METHOD,$this->id);
        pinpoint_add_clues(PP_PHP_ARGS,print_r($this->args, true),$this->id);
    }

    public function onEnd(&$ret){
        pinpoint_add_clues(PP_PHP_RETURN,print_r($this->args, true),$this->id);
    }

    public function onException($e){
        pinpoint_add_clue(PP_ADD_EXCEPTION,$e->getMessage(),$this->id);
    }
}
