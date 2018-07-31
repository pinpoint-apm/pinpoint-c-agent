<?php
/**
 * Copyright 2018 NAVER Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

namespace testNameSpace\demo;

include "test_func_Sam.php";

use testNameSpace\sam\Circle;
use testNameSpace\sam\Square;

class namespace_demo{
//    function __construct()
//    {
//
//    }

    public function getCircleArea(){
        $sam = new Circle(13);
        return $sam->area();
    }

    public function getSquareArea(){
        $sam = new Square(12);
        return $sam->area();
    }

}

$demo = new namespace_demo();
$demo->getCircleArea();
// echo $demo->getSquareArea();
?>