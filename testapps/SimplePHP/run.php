<?php

/**
 * 1. if you have questions,please create an issue on https://github.com/pinpoint-apm/pinpoint-c-agent/issues
 * 2. if need more examples, please let us known by creating an issue.
 * */

require_once __DIR__ . '/vendor/autoload.php';

define('APPLICATION_NAME', 'cd.dev.test.php'); // your application name
define('APPLICATION_ID', 'cd.dev.test.run');  // your application id
define('PP_REQ_PLUGINS', Pinpoint\Plugins\DefaultRequestPlugin::class);
require_once __DIR__ . '/vendor/pinpoint-apm/pinpoint-php-aop/auto_pinpointed.php';

$mysql_host = "dev-mysql";
$mongodb_host = "mongodb";
$mariadb_host = "dev-mariadb";
$memcached_host = "memcached";

// $host = "10.10.10.10";
// $mysql_host = $host;
// $mongodb_host = $host;
// $mariadb_host = $host;
// $memcached_host = $host;

function call_mysql()
{
    global $mysql_host;
    $username = "root";
    $password = "password";
    $dbname = "employees";

    $conn = new PDO("mysql:host=$mysql_host;port=3306;dbname=$dbname", $username, $password);
    $stmt = $conn->prepare("SELECT * FROM `dept_emp_latest_date` LIMIT 1000;");
    $stmt->execute();
    $stmt->fetchAll();
    $stmt->fetch();

    $sql = 'SELECT dept_no, dept_name FROM departments ORDER BY dept_name LIMIT 5';
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
    $mysqli = new mysqli();

    $mysqli->connect($mysql_host, "root", "password", "employees", 3306);

    $result = $mysqli->query("SELECT * FROM `dept_emp_latest_date` LIMIT 1000;");
    printf("Select returned %d rows.\n", $result->num_rows);

    $stmt = $mysqli->prepare("SELECT * FROM employees WHERE emp_no=?");
    $id = 10005;
    $stmt->bind_param("i", $id);
    $stmt->execute();
    $stmt->fetch();
    $stmt->free_result();
    mysqli_query($mysqli, "SELECT * FROM `dept_emp_latest_date` LIMIT 1000;");

    $s_stmt = mysqli_prepare($mysqli, "SELECT * FROM employees WHERE emp_no=?");
    mysqli_stmt_bind_param($s_stmt, "i", $id);
    mysqli_stmt_execute($stmt);
}

function call_mariadb()
{
    global $mariadb_host;
    mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);
    // 33061 , is the port of mariadb server
    $mysqli = new mysqli($mariadb_host, "root", "password", "test", 3307);

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

function check_pinpoint_header($values)
{
    $headers = $values["headers"];
    assert($headers["Pinpoint-Flags"] != "");
    assert($headers["Pinpoint-Host"] != "");
    assert($headers["Pinpoint-Pappname"] != "");
    assert($headers["Pinpoint-Papptype"] != "");
    assert($headers["Pinpoint-Pspanid"] != "");
    assert($headers["Pinpoint-Sampled"] != "");
    assert($headers["Pinpoint-Traceid"] != "");
    assert($headers["Pinpoint-Spanid"] != "");
    assert($headers["User-Abc"] === "xxxx");
    assert($headers["User-Header"] == "2133");
}

function call_curl()
{
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, "http://httpbin.org/anything");
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_HTTPHEADER, [
        'User-Abc:xxxx',
        'User-Header:2133'
    ]);
    $response = curl_exec($ch);
    $j_res = json_decode($response, true);
    check_pinpoint_header($j_res);
    $error = curl_error($ch);
    echo curl_getinfo($ch, CURLINFO_EFFECTIVE_URL);
    // reuse $ch
    curl_setopt($ch, CURLOPT_URL, "http://httpbin.org/get");
    $response = curl_exec($ch);
    $j_res = json_decode($response, true);
    check_pinpoint_header($j_res);
    $error = curl_error($ch);
    echo curl_getinfo($ch, CURLINFO_EFFECTIVE_URL);
    echo "error:$error \n";
    curl_close($ch);
}

function call_redis()
{
    echo "case: strings \n";
    $redis = new Redis();
    $redis->pconnect('redis', 6379);
    $redis->get("a");
    $redis->set("a", "b");
    $redis->setNx("a", "10");
    $redis->append("ar", "aa");
    $redis->setRange("ar", 0, "bb");
    $redis->get("ar");
    $redis->getRange("ar", 0, 2);
    $redis->strlen("ar");
    $redis->getBit("ar", 1);
    $redis->setBit("ar", 1, 3);
    var_dump($redis->get("ar"));
    $redis->mSet(['foo' => 'foo', 'bar' => 'bar', 'baz' => 'baz']);


    $redis->multi()
        ->set('key1', 'val1')
        ->get('key1')
        ->set('key2', 'val2')
        ->get('key2')
        ->exec();

    var_dump($redis->exists('foo', 'bar', 'baz', 'not exists'));
    $redis->flushdb();
}

function call_memcached()
{
    $Memcached = new Memcached();
    global $memcached_host;
    $Memcached->addServer($memcached_host, 11211);
    $Memcached->set('key', "abc");
    var_dump($Memcached->get('key'));       // boolean false
    var_dump($Memcached->getResultCode());  // int 0 which is Memcached::RES_SUCCESS
    var_dump($Memcached->add("test_add", 234));  // int 0 which is Memcached::RES_SUCCESS
    // var_dump($Memcached->appendByKey("xxx", "test_add", 234));  // int 0 which is Memcached::RES_SUCCESS
    var_dump($Memcached->delete("test_add"));  // int 0 which is Memcached::RES_SUCCESS
    var_dump($Memcached->deleteMulti(["test_add", "a", "b", "c"]));  // int 0 which is Memcached::RES_SUCCESS
}

function call_apc()
{
    $bar = 'BAR';
    apcu_add('foo', $bar);
    var_dump(apcu_fetch('foo'));
    echo "\n";
    $bar = 'NEVER GETS SET';
    apcu_add('foo', $bar);
    var_dump(apcu_fetch('foo'));


    apcu_store('cas', 2);
    apcu_cas("cas", 1, 2);
    apcu_cas("cas", 2, 1);
    apcu_inc("cas");
    var_dump(apcu_fetch("cas"));
    apcu_clear_cache();
}

function main()
{
    call_mysql();
    call_mysqli();
    call_mariadb();
    call_mongodb();
    call_curl();
    call_redis();
    call_memcached();
    call_apc();
}

main();
