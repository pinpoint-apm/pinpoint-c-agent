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

// if (!function_exists('getallheaders'))
// { 
//     function getallheaders() 
//     { 
//        $headers = []; 
//        foreach ($_SERVER as $name => $value) 
//        { 
//            if (substr($name, 0, 5) == 'HTTP_') 
//            { 
//                $headers[str_replace(' ', '-', ucwords(strtolower(str_replace('_', ' ', substr($name, 5)))))] = $value; 
//            } 
//        } 
//        return $headers; 
//     } 
// } 
// var_dump(getallheaders());

echo date("Y/m/d");
md5("abcdefg");
phpinfo();
?>