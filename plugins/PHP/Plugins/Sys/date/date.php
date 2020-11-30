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
namespace Plugins\Sys\date;
use Plugins\Common\CommonPlugin;

function date () {
    $args = \pinpoint_get_func_ref_args();

    $plugins = new CommonPlugin('date', null, $args);
    try {
        $plugins->onBefore();
        $ret = call_user_func_array('date', $args);
        $plugins->onEnd($ret);
        return $ret;
    } catch (\Exception $e) {
        $plugins->onException($e);
        throw $e;
    }

}