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

class test_generator{
    function generator($start, $limit, $step=1){
        if($start > $limit){
            throw new LogicException("start cannot bigger that limit.");
        }
        usleep(120000);
        for($i = $start; $i<=$limit; $i += $step){
//            try{
//                yield $i;
//            }catch (Exception $exception){
//                echo $exception->getMessage();
//            }

            echo $i;
        }

    }
}

$test = new test_generator();

echo 'Single digit odd numbers from range():  ';
//foreach ($test->generator(1, 100, 2) as $number) {
//    echo "$number ";
//}

$test->generator(1, 100, 2);
echo "\n";