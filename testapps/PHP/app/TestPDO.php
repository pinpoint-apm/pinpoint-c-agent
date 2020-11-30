<?php
/**
 * User: eeliu
 * Date: 1/4/19
 * Time: 2:32 PM
 */

namespace app;
use Exception;
use \PDO;
use app\User;


class TestPDO
{
    private $con;

    public function connectDb()
    {
        $dsn = 'mysql:host=dev-mysql;port=3306;dbname=pinpoint';
        $this->con = new PDO($dsn, "root","root");
    }

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
        $this->con->query("use pinpoint;");
//        $ret = array();
        $sql = 'illegal sql';
        $ret = $this->con->query($a);
        $data = $ret->fetchAll();
        return $data;
    }

    public function getData3($a)
    {
        $this->con->query("use pinpoint;");
        $sth = $this->con->prepare($a);
        $ret = $sth->execute();
        return $ret;
    }

    public function testerror($a)
    {
        $this->con->exec("INSERT INTO bones(skull) VALUES ('lucy')");
        print $this->con->errorCode();
    }

    public function testPODStatement()
    {
        $sql = "select number,name,email from puser where user_id=:user_id";
        $sth = $this->con->prepare($sql);
        $code = '1';
        $sth->bindParam(':user_id', $code, PDO::PARAM_STR,12);
        $sth->execute();
        
        $sth->bindColumn(1, $name_out);
        $sth->bindColumn(2, $code_out);

        $result = $sth->fetchAll(PDO::FETCH_BOUND);
        var_dump($code_out);
        var_dump($name_out);
        var_dump($result);
    }
}