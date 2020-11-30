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
namespace Plugins\Framework\Swoole\Http;


use Plugins\Common\InsWrapper;

class Response  extends \swoole_http_response
{
    protected $origin;
    public function __construct(&$origin)
    {
        $this->origin = $origin;
    }

    public function status($http_code,$reason=NULL)
    {
        echo "------$http_code----------------";

        return $this->origin->status($http_code,$reason);
    }

//    public function __call($name, $arguments)
//    {
//        return call_user_func_array([$this->origin,$name],$arguments);
//    }
//
//    public static function __callStatic($name, $arguments)
//    {
//       throw new \Exception(";;;;;");
//    }
}