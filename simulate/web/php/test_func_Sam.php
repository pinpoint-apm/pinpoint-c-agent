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

namespace testNameSpace\sam;

interface Sharp{
    public function area();
    public function perimeter();
}

class Square implements Sharp{
    private $length;

    function __construct($length)
    {
        $this->length = $length;
    }

    public function area()
    {
        return pow($this->length,2);
    }

    public function perimeter()
    {
        return $this->length*4;
    }
}

class Circle implements Sharp{
    private $radius;
    const PI = 3.1415926;

    function __construct($radius)
    {
        $this->radius = $radius;
    }

    public function area()
    {
        return pow($this->radius,2)*Circle::PI;
    }

    public function perimeter()
    {
        return 2*$this->radius*Circle::PI;
    }
}

$sharp = new Circle(12);
echo $sharp->area()."<br/>";
echo $sharp->perimeter()."<br/>";
