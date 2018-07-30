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


/**
 * Global function callback
 */
//function fnCallback($msg1, $msg2){
//    echo "msg1:".$msg1."<br/>";
//    echo "msg2:".$msg2."<br/>";
//}
//
//$fnName = "fnCallback";
//$params = array("hello","world");
//
//call_user_func_array($fnName, $params);


/**
 * static function callback
 */
//class MyClass{
//    static function fnCallBack($msg1, $msg2){
//        echo "msg1:".$msg1."<br/>";
//        echo "msg2:".$msg2."<br/>";
//    }
//}
//
//$className = "MyClass";
//$fnName = "fnCallBack";
//$params = array("hello", "world");
//call_user_func_array(array($className, $fnName), $params);

/**
 * member function callback
 */

class MyClass{
    private $name = 'abc';

    public function fnCallBack($msg1='default msg1', $msg2 = 'default msg2'){
        echo 'object name:'.$this->name;
        echo "<br />\n";
        echo 'msg1:'.$msg1;
        echo "<br />\n";
        echo 'msg2:'.$msg2;
        return 1;
    }
}

$myObj = new MyClass();
$fnName = "fnCallBack";
$params = array('hello', 'world');

//construct anonymous function.
$strParams = '';
$strCode = 'global $myObj;global $fnName;global $params; return $myObj->$fnName(';
for ( $i = 0 ; $i < count( $params ) ; $i ++ )
{
    $strParams .= ( '$params['.$i.']' );
    if ( $i != count( $params )-1 )
    {
        $strParams .= ',';
    }
}
$strCode = $strCode.$strParams.");";
$anonymous = create_function( '' , $strCode);

$x = $anonymous();
echo $x;
