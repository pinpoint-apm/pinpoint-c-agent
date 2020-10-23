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

/**
 * Created by PhpStorm.
 * User: eeliu
 * Date: 10/15/20
 * Time: 2:37 PM
 */

namespace Plugins\AutoGen\workerman;


class Context
{
    private static $context=null;
    private $id;
    public static function getInstance()
    {
        if(self::$context == null){
            self::$context = new Context();
        }
        return self::$context;
    }

    public function setId($id)
    {
        assert($id!=0);
        $this->id = $id;
    }
    public function getId()
    {
        return $this->id;
    }
}