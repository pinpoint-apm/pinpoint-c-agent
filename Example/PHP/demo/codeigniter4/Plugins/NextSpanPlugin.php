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


namespace Plugins;
use Plugins\Candy;


///@hook:CodeIgniter\HTTP\CURLRequest::send
class NextSpanPlugin extends Candy
{
    ///@hook:CodeIgniter\HTTP\CURLRequest::request
    public function onBefore(){
        if(strpos($this->apId, "request"))
        {
            //todo append the pinpoint header
            return ;
        }

        if(strpos($this->apId, "send"))
        {
            pinpoint_add_clue(DESTINATION,parse_url($this->args[1],PHP_URL_PATH));
            pinpoint_add_clue(SERVER_TYPE,PINPOINT_PHP_REMOTE);
            pinpoint_add_clue(NEXT_SPAN_ID,PerRequestPlugins::instance()->getCurNextSpanId());
            pinpoint_add_clues(HTTP_URL,parse_url($this->args[1],PHP_URL_PATH));
        }

    }

    public function onEnd(&$ret)
    {
        if(strpos($this->apId, "send"))
        {
            pinpoint_add_clues(HTTP_STATUS_CODE, (string)$ret->getStatusCode());
        }
    }

    public function onException($e){
        pinpoint_add_clue("EXP",$e->getMessage());
    }

}