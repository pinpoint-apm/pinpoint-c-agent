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

$redis = new Redis();
$redis->connect('10.34.130.44',6379);
$redis->auth('SanpangJin');
//echo "Server is running: " . $redis->ping();
$redis ->set("test" , "Hello World");
echo $redis ->get("test");

$redis->LPUSH('rank', 30); //(integer) 1
$redis->LPUSH('rank', 56); //(integer) 2
$redis->LPUSH('rank', 42); //(integer) 3
$redis->LPUSH('rank', 22); //(integer) 4
$redis->LPUSH('rank', 0);  //(integer) 5
$redis->LPUSH('rank', 11); //(integer) 6
$redis->LPUSH('rank', 32); //(integer) 7
$redis->LPUSH('rank', 67); //(integer) 8
$redis->LPUSH('rank', 50); //(integer) 9
$redis->LPUSH('rank', 44); //(integer) 10
$redis->LPUSH('rank', 55); //(integer) 11

var_dump($redis->LRANGE ('rank',0,-1));

print_r($redis->keys('*'));
$redis->delete('rank');
$redis->delete("test");
echo "ok";
echo "<br>";
echo  time();

?>