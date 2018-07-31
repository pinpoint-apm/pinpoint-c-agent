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

class test_exception{
    public $ret=3;
    public function divide($num){
        try{
            //PHP thinks devide by 0 is a warning, but not an exception.
//            $this->ret = 4/$num;
            $this->ret = 111;
            throw new Exception("Value must be bigger than 1!");
        }catch (Exception $exception){
            echo $exception->getMessage()."<br/>";
        }
//        $aa = array("volvo", 12, 343.1);
//        var_dump($aa);
//        usleep(120000);

        return $this->ret;
    }
}

$e = new test_exception();
echo "result is: ".$e->divide(0)."<br/>";
?>