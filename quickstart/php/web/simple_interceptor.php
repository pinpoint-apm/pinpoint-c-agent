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

define("GREETING","Hello you! How are you today?");

function xxxx()
{
  echo "xxxx";
}

class SObject
{
  var $x_;

  public function __construct($x)
  {
    $this->x_ = $x;
  }
};

class SObjectString
{
  var $x_;

  public function __construct($x)
  {
    $this->x_ = $x;
  }

  public function __toString()
  {
    return $this->x_;
  }

};

function test_simple_interceptor($null_arg, $long_arg, $double_arg, $bool_arg,
                                 $array_arg, $object_arg, $object_tostring_arg, $string_arg, $resource_arg, $constant_arg)
{
  return "return xxx";
}

$myfile = fopen("simple_interceptor.php", "r");

test_simple_interceptor(null, 123, 3.1415, true, array(1, 2), new SObject(1), new SObjectString(1.67), "abcd",
    $myfile, constant("GREETING"));
fclose($myfile);

call_user_func("xxxx");

call_user_func(function ($a) {
  echo $a;
}, "hello anonymous function");
?>