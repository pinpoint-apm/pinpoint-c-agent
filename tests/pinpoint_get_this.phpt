--TEST--
pinpoint_php test get this
--SKIPIF--
<?php if (!extension_loaded("pinpoint_php")) print "skip"; ?>
--INI--
pinpoint_php.DebugReport=true
--FILE--
<?php 

class Foo{
    public function output(){

      $onBefore = function(){
        $f = pinpoint_get_this();
        if($f instanceof  Foo){
          var_dump($f->abc);
          var_dump($f->abc_s);
          echo " update foo members \n";
          $f->abc = 1234;
          $f->abc_s= "hello pinpoint";
        }
      };

      $onBefore();
    }
    public $abc = 123;
    public $abc_s = "123";
}

$f = new Foo();
$f->output();
var_dump($f->abc);
var_dump($f->abc_s);
var_dump(pinpoint_get_this());

echo "test static method \n";
class S_Foo
{
    public static $my_static = 'foo';

    public static function staticValue() {
      $f = pinpoint_get_this();
      var_dump($f);
      return self::$my_static;
    }
}

$sf = new S_Foo();
S_Foo::staticValue();

--EXPECTF--
int(123)
string(3) "123"
 update foo members 
int(1234)
string(14) "hello pinpoint"
[pinpoint] [%d] [%d]pinpoint_get_this: not support, no this
bool(false)
test static method 
[pinpoint] [%d] [%d]pinpoint_get_this: not support, no this
bool(false)