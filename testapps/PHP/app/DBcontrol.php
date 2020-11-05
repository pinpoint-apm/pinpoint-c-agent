<?php
/**
 * User: eeliu
 * Date: 1/4/19
 * Time: 2:32 PM
 */

namespace app;
use Exception;
use \PDO;



class DBcontrol
{
    private $servername;//= "dev-test-mysql:3306";

    private $username ;// = "root";

    private $password ;//= "root";

    private $con;

    public function __construct($dburl, $dbname, $dbpasswd)
    {
        $this->password = $dbpasswd;
        $this->username = $dbname;
        $this->servername = $dburl;
    }

    public function connectDb()
    {
        $this->con = new PDO("mysql:host=".$this->servername, $this->username,$this->password);
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

    /**
     * @param $a
     * @return array
     */
    public function getData2()
    {
        $ret = array();
        $sql = 'illegal sql';
        $ret[] = $this->con->exec($sql) ;
        $this->con->errorCode();
        return $ret;
    }
}