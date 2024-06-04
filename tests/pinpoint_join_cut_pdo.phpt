--TEST--
pinpoint_php pinpoint_join_cut_pdo
--SKIPIF--
<?php
if (!extension_loaded("pinpoint_php"))
  print "skip";
if (!extension_loaded("pdo"))
  print "skip";
if (version_compare(phpversion(), '8.2.0', '>=')){
  print "skip";
  print phpversion() . ">= 8.2.0";
}
  
   
?>
--INI--
pinpoint_php.DebugReport=true
--EXTENSIONS--
pdo_mysql
--FILE--
<?php

_pinpoint_join_cut(
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
    }
);
$pdo_exec = "PDO::exec";
_pinpoint_join_cut(
    ["PDO", "exec"],
    function ($statement) use ($pdo_exec) {
        echo "$pdo_exec: on_before: $statement \n";
    },
    function ($ret) use ($pdo_exec) {
        echo "$pdo_exec : on_end \n";
    },
    function ($e) use ($pdo_exec) {
        echo "on_exception \n";
    }
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


--EXPECTF--
[pinpoint] [%d] [%d]try to interceptor module(class)/function=pdo:__construct
[pinpoint] [%d] [%d]added interceptor on `module`: pdo::__construct success
[pinpoint] [%d] [%d]try to interceptor module(class)/function=pdo:exec
[pinpoint] [%d] [%d]added interceptor on `module`: pdo::exec success
case: pdo() 
[pinpoint] [%d] [%d]pinpoint_interceptor_handler_entry: handle func/method:pdo::__construct
on_before: mysql:host=dev-mysql;dbname=employees 
attached dsn 
[pinpoint] [%d] [%d] call_interceptor_before:pdo::__construct return type(1) zval
[pinpoint] [%d] [%d]replace_ex_caller_parameters return value must be `an array`
on_end 
[pinpoint] [%d] [%d]call_interceptor_end: pdo::__construct 
[pinpoint] [%d] [%d]pinpoint_interceptor_handler_entry: handle func/method:pdo::exec
PDO::exec: on_before: CREATE table IF NOT EXISTS pdo_test(
     ID INT( 11 ) AUTO_INCREMENT PRIMARY KEY,
     Name VARCHAR( 250 ) NOT NULL,
     AGE tinyint(1) unsigned NOT NULL default '1' ); 
[pinpoint] [%d] [%d] call_interceptor_before:pdo::exec return type(1) zval
[pinpoint] [%d] [%d]replace_ex_caller_parameters return value must be `an array`
PDO::exec : on_end 
[pinpoint] [%d] [%d]call_interceptor_end: pdo::exec 
[pinpoint] [%d] [%d]pinpoint_interceptor_handler_entry: handle func/method:pdo::exec
PDO::exec: on_before: INSERT INTO pdo_test (NAME, AGE)
VALUES ('a',1),('b',1),('c',3),('d',1); 
[pinpoint] [%d] [%d] call_interceptor_before:pdo::exec return type(1) zval
[pinpoint] [%d] [%d]replace_ex_caller_parameters return value must be `an array`
PDO::exec : on_end 
[pinpoint] [%d] [%d]call_interceptor_end: pdo::exec 
int(4)
object(PDOStatement)#8 (1) {
  ["queryString"]=>
  string(22) "SELECT * FROM pdo_test"
}
object(PDOStatement)#8 (1) {
  ["queryString"]=>
  string(22) "SELECT * FROM pdo_test"
}
object(PDOStatement)#8 (1) {
  ["queryString"]=>
  string(22) "SELECT * FROM pdo_test"
}
object(PDOStatement)#8 (1) {
  ["queryString"]=>
  string(22) "SELECT * FROM pdo_test"
}
bool(true)
array(2) {
  ["name"]=>
  string(1) "a"
  [0]=>
  string(1) "a"
}
array(2) {
  [0]=>
  array(2) {
    ["name"]=>
    string(1) "b"
    [0]=>
    string(1) "b"
  }
  [1]=>
  array(2) {
    ["name"]=>
    string(1) "d"
    [0]=>
    string(1) "d"
  }
}
string(3) "123"
string(37) "mysql:host=dev-mysql;dbname=employees"
[pinpoint] [%d] [%d]pinpoint_interceptor_handler_entry: handle func/method:pdo::exec
PDO::exec: on_before: DROP TABLE pdo_test 
[pinpoint] [%d] [%d] call_interceptor_before:pdo::exec return type(1) zval
[pinpoint] [%d] [%d]replace_ex_caller_parameters return value must be `an array`
PDO::exec : on_end 
[pinpoint] [%d] [%d]call_interceptor_end: pdo::exec 
[pinpoint] [%d] [%d]start free interceptor: pdo::__construct
[pinpoint] [%d] [%d]start free interceptor: pdo::exec