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
namespace Plugins\AutoGen\workerman;
use Workerman\MySQL\Connection as MysqlConnection;
/**
 * Class MysqlPlugin
 * @hook: Workerman\MySQL\Connection::execute
 */
class MysqlPlugin extends Candy
{

    function onBefore()
    {
        $query = $this->args[0];
//        $parameters = $this->args[1];
        pinpoint_add_clue(PP_SERVER_TYPE,PP_MYSQL,$this->pinpoint_id);
        assert($this->who instanceof MysqlConnection);
        $setting = $this->getSetting($this->who);
        $dst = $setting['host'].":".$setting['port'];
        pinpoint_add_clue(PP_DESTINATION,$dst,$this->pinpoint_id);
        pinpoint_add_clue(PP_SQL_FORMAT,$query,$this->pinpoint_id);

    }

    function onEnd(&$ret)
    {


        parent::onEnd($ret);
    }

    function onException($e)
    {

    }

    function getSetting($conn)
    {
        $getSetting = function (){
            return $this->settings;
        };

        return $getSetting->call($conn);
    }

}