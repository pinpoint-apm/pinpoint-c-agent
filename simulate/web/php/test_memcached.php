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

// $memcache_obj = memcache_connect("dev-mysql-host", 11211);

// memcache_add($memcache_obj, 'var_key', 'test variable', false, 30);

// $memcache_obj->add('var_key', 'test variable', false, 30);
// echo "ok";

$memcache = new Memcache();

if(!$memcache->connect("dev-mysql-host", 11211))
{
    echo "peer connect failed";
    exit(0);
}

$keyarry = range(0,100);

foreach ($keyarry as  $value) {
    $memcache->add($value,"........................................",true,30);
}

foreach ($keyarry as  $value) {
    $memcache->delete($value);
}

$memcache->close();


?>