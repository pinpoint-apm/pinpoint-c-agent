--TEST--
app info 
--SKIPIF--
<?php if (!extension_loaded("pinpoint_php")) print "skip"; ?>
--INI--

--FILE--
<?php 

## test in function
function foo(&$a,$b,$c,$d)
{
    $ar = \pinpoint_get_func_ref_args();
    var_dump($ar);
    $ar[0] = "hello php!!!";
    $ar[1] = 10;
    $ar[2] = 10;
    $ar[3] = 10;
    
}
$var = 100;
foo($var,1,3,4);
var_dump($var);
## global
var_dump(\pinpoint_get_func_ref_args());

## test in class
class Foo{
    public function __construct( $a,$b)
    {
        echo "call __construct \n";
        var_dump(\pinpoint_get_func_ref_args());
    }

    public function __destruct()
    {
        echo "call __destruct \n";
        var_dump(\pinpoint_get_func_ref_args()); 
    }

    public function test_none()
    {
        var_dump(\pinpoint_get_func_ref_args()); 
    }

    public function foo(string $a,int $b,array $ar,$c = 'hello pinpoint-php',$d=123.4534,$e=null)
    {
       $input = \pinpoint_get_func_ref_args();
       var_dump($input);
    }

    public static function static_foo(string $a,int $b,array $ar,$c = 'hello pinpoint-php',$d=123.4534,$e=null)
    {
       $var =  \pinpoint_get_func_ref_args();
       var_dump($var);
    }

    public  function return_all($a,$b,$c)
    {
        return \pinpoint_get_func_ref_args();
    }

}

$foo = new Foo(1,2);
$foo->foo('hello pinpoint',1,array('1'=>2,'c'=>3,'e'=>5));
Foo::static_foo('hello pinpoint',1,array('1'=>2,'c'=>3,'e'=>5));
$foo->test_none();
var_dump($foo->return_all("23",1,null));

echo "test anonymous \n";
$name = 'eeliu';
$nobody = function ($arg) use (&$name) {
    var_dump(\pinpoint_get_func_ref_args());
    $name = 'eeliu-01';
    $arg = $arg.$name;
    return \pinpoint_get_func_ref_args();
};
var_dump($nobody("hello "));
echo "\n";

echo "test pinpoint_get_func_ref_args call in call_user_func_array \n";

function foo_ref($a,&$b,&$c)
{
    var_dump(func_get_args());
    $b = 10;
    $c = 11;
    return array($b,$c);
}

function aop_foo_ref($a,&$b,&$c)
{
    $args = \pinpoint_get_func_ref_args();
    return call_user_func_array('foo_ref',$args);
}
$b =2;
$c = 12;
var_dump(aop_foo_ref("adf",$b,$c));
echo "b = $b \n";
echo "c = $c \n";

--EXPECTF--
array(4) {
  [0]=>
  &int(100)
  [1]=>
  int(1)
  [2]=>
  int(3)
  [3]=>
  int(4)
}
string(12) "hello php!!!"

Warning: pinpoint_get_func_ref_args():  Called from the global scope - no function context in %s on line 18
bool(false)
call __construct 
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
array(3) {
  [0]=>
  string(14) "hello pinpoint"
  [1]=>
  int(1)
  [2]=>
  array(3) {
    [1]=>
    int(2)
    ["c"]=>
    int(3)
    ["e"]=>
    int(5)
  }
}
array(3) {
  [0]=>
  string(14) "hello pinpoint"
  [1]=>
  int(1)
  [2]=>
  array(3) {
    [1]=>
    int(2)
    ["c"]=>
    int(3)
    ["e"]=>
    int(5)
  }
}
array(0) {
}
array(3) {
  [0]=>
  string(2) "23"
  [1]=>
  int(1)
  [2]=>
  NULL
}
test anonymous 
array(1) {
  [0]=>
  string(6) "hello "
}
array(1) {
  [0]=>
  string(14) "hello eeliu-01"
}

test pinpoint_get_func_ref_args call in call_user_func_array 
array(3) {
  [0]=>
  string(3) "adf"
  [1]=>
  int(2)
  [2]=>
  int(12)
}
array(2) {
  [0]=>
  int(10)
  [1]=>
  int(11)
}
b = 10 
c = 11 
call __destruct 
array(0) {
}