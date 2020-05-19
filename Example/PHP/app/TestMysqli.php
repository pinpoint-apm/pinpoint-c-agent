<?php
/**
 * User: eeliu
 * Date: 1/4/19
 * Time: 2:32 PM
 */

namespace app;
use Exception;
use \mysqli;



class TestMysqli
{
    private $con;

    public function connectDb()
    {echo "onbefore";
    echo $dsn = 'localhost:3306';
    try{
        $this->con = new mysqli($dsn, "","", "");
        if(mysqli_connect_error()){
            echo mysqli_connect_error();
        }
    }catch (Exception $e){echo "exp";}
    finally{echo "end";}}

    public function getData1()
    {
        $ret = array();
        $sql = 'show databases;';
        foreach ($this->con->query($sql) as $row) {
            $ret[] = $row;
        }
        return $ret;
    }

    public function getData2($a)
    {
        $ret = array();
        $sql = 'illegal sql';
        $ret = $this->con->query($a);
        $data = $ret->fetch_all();
        return $data;
    }

}