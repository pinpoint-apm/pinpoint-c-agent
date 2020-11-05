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

/*
 * User: eeliu
 * Date: 11/5/20
 * Time: 4:17 PM
 */

namespace Plugins\Sys\phpredis;


class Redis extends \Redis
{
    public function connect( $host, $port = 6379, $timeout = 0.0, $reserved = null, $retry_interval = 0 )
    {
        $p = new ConnectPlugin("Redis::".__FUNCTION__,$this,$host, $port, $timeout, $reserved, $retry_interval);
        try{
            $p->onBefore();
            $r = parent::connect($host, $port, $timeout, $reserved, $retry_interval);
            $p->onEnd($r);
            return $r;
        }catch (\Exception $e)
        {
            $p->onException($e);
            throw $e;
        }

    }

    public function get( $key )
    {
        $p = new GetPlugin("Redis::".__FUNCTION__,$this,$key);
        try{
            $p->onBefore();
            $r = parent::get($key);
            $p->onEnd($r);
            return $r;
        }catch (\Exception $e)
        {
            $p->onException($e);
            throw $e;
        }
    }

    public function set($key, $value, $timeout = null)
    {

        $p = new SetPlugin("Redis::".__FUNCTION__,$this,$key, $value, $timeout);
        try{
            $p->onBefore();
            $r = parent::set($key, $value, $timeout);
            $p->onEnd($r);
            return $r;
        }catch (\Exception $e)
        {
            $p->onException($e);
            throw $e;
        }

    }

}