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
namespace Plugins\Sys\mysqli;

class Mysqli extends \mysqli
{
    public function prepare ($query)
    {
        $plugin = new MysqliPreparePlugin("Mysqli::prepare",$this,...$query);
        try{
            $plugin->onBefore();
            $ret = parent::prepare($query);
            $plugin->onEnd($ret);
            return $ret;

        }catch (\Exception $e)
        {
            $plugin->onException($e);
        }
    }

    public function query ($query, $resultmode = MYSQLI_STORE_RESULT)
    {
        $plugin = new MysqliQueryPlugin("Mysqli::query",$this,$query, $resultmode);
        try{
            $plugin->onBefore();
            $ret = parent::query($query,$resultmode);
            $plugin->onEnd($ret);
            return $ret;
//            return  new ProfilerMysqliResult($ret);

        }catch (\Exception $e)
        {
            $plugin->onException($e);
        }
    }
}

function mysqli_init() {
    return new Mysqli();
}
