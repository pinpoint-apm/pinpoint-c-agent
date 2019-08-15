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
    {echo "onbefore";
    echo $dsn = 'mysql:host=localhost;port=3306';
    try{  $this->con = new PDO($dsn, "","");}catch (Exception $e){echo "exp";}
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
        $this->con->query("use DBTest;");
//        $ret = array();
        $sql = 'illegal sql';
        $ret = $this->con->query($a);
        $data = $ret->fetchAll();
        return $data;
    }

    public function getData3($a)
    {
        $this->con->query("use DBTest;");
//        $sth = $this->con->prepare($a);
        $ret = $this->con->exec($a);

        return $ret;
    }

    public function test()
    {
        $this->con->beginTransaction();
        $this->con->getAttribute(PDO::ATTR_CONNECTION_STATUS);
        $this->con->exec("use DBTest;");
        $sql1 = 'INSERT INTO user (name) VALUES (?)';
        $sth1 = $this->con->prepare($sql1);
        $sth1->execute(array("Evy"));
        $this->con->commit();

        $n = null;
        $sql2 = "SELECT name FROM user where name='xiaogao'";
        $sth2 = $this->con->prepare($sql2);
        $sth2->bindParam(1, $n);
        $sth2->execute();
        $result = $sth2->fetch(PDO::FETCH_BOUND);
        var_dump($result);
        $user = $sth2->fetchObject('app\User');
        var_dump($user);
    }

    public function testerror($a)
    {
        $this->con->exec("INSERT INTO bones(skull) VALUES ('lucy')");
        print $this->con->errorCode();
    }
}