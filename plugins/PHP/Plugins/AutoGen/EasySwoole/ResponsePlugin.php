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
namespace Plugins\AutoGen\EasySwoole;

use Plugins\Framework\Swoole\IDContext;

/**
 * @hook: EasySwoole\Http\Message\Response::withStatus
 */
class ResponsePlugin
{
    public function __construct($apId,$who,&...$args)
    {
        $id = IDContext::get();
        $code = $args[0];
        pinpoint_set_context((string)PP_HTTP_STATUS_CODE,(string)$code,$id);
        if($code !== 200){
            pinpoint_add_clue(PP_ADD_EXCEPTION,"http response not 200. status=[$code]",$id,PP_ROOT_LOC);
        }
    }

    function onBefore()
    {
    }

    function onEnd(&$ret)
    {

    }

    function onException($e)
    {

    }
}