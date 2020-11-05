<?php
namespace app;

class Foo
{
    /// test 0 parameter
    public function foo0()
    {
        $dbms='mysql';     //Êý¾Ý¿âÀàÐÍ
        $host='localhost'; //Êý¾Ý¿âÖ÷»úÃû
        $dbName='test';    //Ê¹ÓÃµÄÊý¾Ý¿â
        $user='root';      //Êý¾Ý¿âÁ¬½ÓÓÃ»§Ãû
        $pass='';          //¶ÔÓ¦µÄÃÜÂë
        $dsn="$dbms:host=$host;dbname=$dbName";

        $mysql = new \PDO($dsn, $user, $pass);
    }

    /// test 1 parameter
    public function foo_p1($p)
    {


//        return $ch;
    }

    /// test 1 parameter
    public function foo_p2($p1,$p2)
    {

    }

    /// test 1 parameter
    public function foo_p3($p1,$p2,$p3)
    {
        echo "$p1 , $p2, $p3";
        return $p1;
    }

    /// test 1 parameter
    public function foo_p3_rbool($p1,$p2,$p3):bool
    {
        $args = func_num_args();
        echo __METHOD__.":count=".count($args).__LINE__.__FUNCTION__.__NAMESPACE__.__CLASS__."\n";
        return false;
    }

    /// test 1 parameter
    public function foo_p3_rfloat($p1,$p2,$p3):float
    {
        return true;
    }
}