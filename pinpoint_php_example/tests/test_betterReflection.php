<?php
/**
 * User: eeliu
 * Date: 1/25/19
 * Time: 5:54 PM
 */


//require_once __DIR__. '/../vendor/autoload.php';
//
//class Test
//{
//    public function __construct($a,$b,$c)
//    {
//    }
//
//}
//
//
//
//$classInfo = (new \Roave\BetterReflection\BetterReflection())
//    ->classReflector()
//    ->reflect(Test::class);
//
//// Retrieves the AST statements array *within* the method's curly braces
//$ast = $classInfo->getMethod('__construct')->getBodyAst();
//echo gettype($classInfo->getMethod('__construct')->getParameters());
//
//class Foo{
//
//    public function __construct()
//    {
//        echo __CLASS__."\n";
//    }
//    public function fout()
//    {
//        echo __FUNCTION__."\n";
//    }
//}
//
//class R_Foo extends Foo{
//    public function out()
//    {
//        echo __CLASS__ ."\n";
//    }
//}
//
//$r =  new R_Foo();
//$r->out();
//$r->fout();

class Foo{

    function out(){
        echo date("Y-m-s");
    }
}

function registerSth($p){
    register_shutdown_function(array($p,'out'));
}

$F =  new Foo();

registerSth($F);