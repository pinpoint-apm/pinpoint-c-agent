<?php

/**
 * 1. if you have questions,please create an issue on https://github.com/pinpoint-apm/pinpoint-c-agent/issues
 * 2. if need more examples, please let us known by issue.
 * */

require_once __DIR__ . '/vendor/autoload.php';

define('AOP_CACHE_DIR', __DIR__ . '/Cache');
define('APPLICATION_NAME', 'cd.dev.test.php'); // your application name
define('APPLICATION_ID', 'cd.dev.test.run');  // your application id
define('PP_REQ_PLUGINS', SimplePHP\RequestPlugin::class);
require_once __DIR__ . '/vendor/pinpoint-apm/pinpoint-php-aop/auto_pinpointed.php';

use Pinpoint\Plugins\Sys\PDO\PDO;
use Pinpoint\Plugins\Sys\mysqli\Mysqli;

$mysql_host = "dev-mysql";
$mongodb_host = "mongodb";
$mariadb_host = "dev-mariadb";
function call_mysql()
{
    global $mysql_host;
    $username = "root";
    $password = "password";
    $dbname = "employees";

    $conn = new PDO("mysql:host=$mysql_host;port=33060;dbname=$dbname", $username, $password);
    $stmt = $conn->prepare("SELECT * FROM `dept_emp_latest_date` LIMIT 1000;");
    $stmt->execute();

    $sql = 'SELECT name, dept_no, dept_name FROM departments ORDER BY dept_name LIMIT 10';
    foreach ($conn->query($sql) as $row) {
        print $row['dept_no'] . "\t";
        print $row['dept_name'] . "\n";
    }

    $conn = null;
}

function call_mysqli()
{
    global $mysql_host;
    mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);
    $mysqli = new mysqli($mysql_host, "root", "password", "employees", 33060);

    $result = $mysqli->query("SELECT * FROM `dept_emp_latest_date` LIMIT 1000;");
    printf("Select returned %d rows.\n", $result->num_rows);

    $stmt = $mysqli->prepare("SELECT * FROM employees WHERE emp_no=?");
    $id = 10005;
    $stmt->bind_param("i", $id);
    $stmt->execute();
    $stmt->fetch();
}

function call_mariadb()
{
    global $mariadb_host;
    mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);
    // 33061 , is the port of mariadb server
    $mysqli = new mysqli($mariadb_host, "root", "password", "test", 33061);

    $result = $mysqli->query("SELECT * FROM `contacts` LIMIT 1000;");
    printf("Select returned %d rows.\n", $result->num_rows);

    $stmt = $mysqli->prepare("INSERT INTO contacts (name,age,email) VALUES(?, ?, ?)");
    $name = "eeliu";
    $age = 18;
    $email = "eeliu@navercorp.com";
    $stmt->bind_param("sis", $name, $age, $email);
    $stmt->execute();
}

function call_mongodb()
{
    global $mongodb_host;
    $client = new MongoDB\Client("mongodb://$mongodb_host:27017");

    $db = $client->test2;
    echo "Database test2 created/selected<br>";

    $coll = $db->selectCollection("mycoll");
    echo "Collection mycoll created/selected<br>";

    $coll->insertOne(['foo' => 'bar']);

    echo "Result:<br>";
    var_dump($coll->findOne(['foo' => 'bar']));
}

function main()
{
    call_mysql();
    call_mysqli();
    call_mariadb();
    call_mongodb();
}

main();
