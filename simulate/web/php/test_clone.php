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

class Television{
    protected $_color = 'black';
    protected $name = "ChangHong";

    public function setColor($color){
        $this->_color = $color;
    }

    /**
     * @return string
     */
    public function getColor()
    {
        return $this->name." color is ".$this->_color;
    }

    public function __clone()
    {
        $this->name = "ClonableName";
    }
}

$tv1 = new Television();
$tv1->setColor("red");
echo $tv1->getColor()."<br/>";

$tv2 = clone $tv1;
$tv2->setColor("white");
echo $tv2->getColor()."<br/>";