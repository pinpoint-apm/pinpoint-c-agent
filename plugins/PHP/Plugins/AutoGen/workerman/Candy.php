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

abstract class Candy
{
    protected $apId;
    protected $who;
    protected $args;
    protected $pinpoint_id;
    protected $ret=null;

    public function __construct($apId,$who,&...$args)
    {
        $parent_id = Context::getInstance()->getId();
        $this->apId = $apId;
        $this->who =  $who;
        $this->args = &$args;
        $newid = pinpoint_start_trace($parent_id);
        // store current id into candy
        $this->pinpoint_id = $newid;
        // update global id
        Context::getInstance()->setId($newid);
        pinpoint_add_clue(PP_INTERCEPTOR_NAME,$apId,$this->pinpoint_id);
        echo "$apId start -> $parent_id $newid \n";
    }

    public function __destruct()
    {

    }

    abstract function onBefore();

    abstract function onException($e);

    function onEnd(&$ret)
    {
        // update pinpoint_id
        $id = pinpoint_end_trace($this->pinpoint_id);
        Context::getInstance()->setId($id);
        echo  $this->apId." end ".$this->pinpoint_id."\n";
    }

}