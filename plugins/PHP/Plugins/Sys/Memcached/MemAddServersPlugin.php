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
 * Time: 3:09 PM
 */

namespace Plugins\Sys\Memcached;
use Plugins\Common\Candy;

class MemAddServersPlugin extends Candy
{

    function onBefore()
    {
        $servers = $this->args[0];
        $url ='';
        foreach ($servers as $server)
        {
            $url.= sprintf("%s:%d ",$server[0],$server[1]);
        }
        pinpoint_add_clue(PP_SERVER_TYPE,PP_MEMCACHED);
        pinpoint_add_clue(PP_DESTINATION,$url);
    }

    function onEnd(&$ret)
    {
        // TODO: Implement onEnd() method.
    }
}