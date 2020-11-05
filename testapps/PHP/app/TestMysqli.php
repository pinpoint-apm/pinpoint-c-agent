<?php
/**
 * User: eeliu
 * Date: 1/4/19
 * Time: 2:32 PM
 */

namespace app;
use mysqli;



class TestMysqli
{
    private $con;

    public function connectDb()
    {
        $dsn = 'dev-mysql:3306';
        $this->con = new mysqli($dsn, "root", "root", "pinpoint");

        if (mysqli_connect_error()) {
            echo mysqli_connect_error();
        }
    }

    public function getData1()
    {
        $sql = 'show databases;';
        foreach ($this->con->query($sql) as $row) {
            $ret[] = $row;
        }
        return $ret;
    }

    public function getData2($a)
    {
        $sql = "select number,name,email from puser where user_id=?";

        $stmt = $this->con->prepare($sql);
        $id = 1;
        $stmt->bind_param("i",$id);
        $stmt->execute();
        $stmt->bind_result($number,$name,$email);
        $stmt->fetch();
        return [$number,$name,$email];
    }

}