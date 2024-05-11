<?php

pinpoint_join_cut(
    ["PDO", "__construct"],
    function ($dsn, $username = null, $password = null, $options = null) {
        echo "on_before: $dsn \n";
        $pdo = pinpoint_get_this();
        if ($pdo instanceof PDO) {
            $pdo->dsn = $dsn;
            echo "attached dsn \n";
        }
    },
    function ($ret) {
        echo "on_end \n";
    },
    function ($e) {
        echo "on_exception \n";
    },
);
$pdo_exec = "PDO::exec";
pinpoint_join_cut(
    ["PDO", "exec"],
    function ($statement) use ($pdo_exec) {
        echo "$pdo_exec: on_before: $statement \n";
    },
    function ($ret) use ($pdo_exec) {
        echo "$pdo_exec : on_end \n";
    },
    function ($e) use ($pdo_exec) {
        echo "on_exception \n";
    },
);


echo "case: pdo() \n";

$dbname = "employees";
$pdo = new PDO("mysql:host=dev-mysql;dbname=$dbname", 'root', 'password');
$pdo->setAttribute(PDO::MYSQL_ATTR_USE_BUFFERED_QUERY, false);
$pdo->abc = "123";
$sql = "CREATE table IF NOT EXISTS pdo_test(
     ID INT( 11 ) AUTO_INCREMENT PRIMARY KEY,
     Name VARCHAR( 250 ) NOT NULL,
     AGE tinyint(1) unsigned NOT NULL default '1' );";
$ret = $pdo->exec($sql);
if ($ret === false) {
    echo "create pdo_test failed" . $pdo->errorCode();
}

$sql = "INSERT INTO pdo_test (NAME, AGE)
VALUES ('a',1),('b',1),('c',3),('d',1);";
$ret = $pdo->exec($sql);
var_dump($ret);

$unbufferedResult = $pdo->query("SELECT * FROM pdo_test");
foreach ($unbufferedResult as $row) {
    var_dump($unbufferedResult);
}

$sth = $pdo->prepare('SELECT name FROM pdo_test
    WHERE AGE = :AGE limit 3');
/* Names can be prefixed with colons ":" too (optional) */
$sth->bindValue(':AGE', 1);
$ret = $sth->execute();
var_dump($ret);
$ret = $sth->fetch();
var_dump($ret);
$ret = $sth->fetchAll();
var_dump($ret);
var_dump($pdo->abc);
var_dump($pdo->dsn);
$sql = 'DROP TABLE pdo_test';
$ret = $pdo->exec($sql);
