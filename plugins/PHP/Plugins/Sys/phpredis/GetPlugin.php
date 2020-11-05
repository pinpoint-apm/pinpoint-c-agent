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
 * Time: 4:32 PM
 */

namespace Plugins\Sys\phpredis;


use Plugins\Common\Candy;

class GetPlugin extends Candy
{

    function onBefore()
    {
        $key = $this->args[0];
        pinpoint_add_clue(PP_SERVER_TYPE,PP_REDIS);
        pinpoint_add_clues(PP_PHP_ARGS,"$key");
    }

    function onEnd(&$ret)
    {
        // TODO: Implement onEnd() method.
    }
}