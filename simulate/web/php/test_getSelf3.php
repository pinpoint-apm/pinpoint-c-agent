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

class test_getSelf3
{
    public $num = 10;
    public $name = "sam";

    public function testFunc(&$arg){
        //        unset($arg);
        $arg = "bbbb";
        try{
            throw new Exception("Error Processing Request", 1);
        }catch(Exception $e){
            echo $e->getMessage()."<br/>";
        }
        return $arg;
    }
}

$a = new test_getSelf3();
echo $a->name."<br/>";
echo $a->testFunc($a->name);
// echo $a->name;
?>