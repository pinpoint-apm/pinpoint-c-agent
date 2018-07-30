<?php
/**
 * Copyright 2018 NAVER Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

$dsn = 'mysql:dbname=test;host=dev-mysql-host';
// $dsn = 'mysql;host=dev-mysql-host';
$user = 'phpuser';
$password = '1234';
try {
    $pdo = new PDO($dsn, $user, $password);
} catch (PDOException $e) {
    echo 'Connection failed: ' . $e->getMessage();
}



$pdo->query("CREATE DATABASE IF NOT EXISTS `DBTest` DEFAULT CHARACTER SET utf8 ");
$pdo->query("use DBTest");
exit;
// create table
$pdo->query("CREATE TABLE IF NOT EXISTS user ( `name` VARCHAR(16) NOT NULL) ");
$pdo->query("truncate table user");

$pdo->query("insert into user (name) values('xiaogao'),('qitongxue'),('houtongxue')");

$sth = $pdo->prepare("SELECT name FROM user");
$sth->execute();
$result = $sth->fetchAll();
print_r($result);

$con = mysql_connect("dev-mysql-host","phpuser","1234");
if (!$con)
{
    echo 'Could not connect: ' . mysql_error();
    exit;
}

$sql = "SELECT * FROM `DBTest.user`";
$result = mysql_query($sql,$con);
print_r(mysql_fetch_array($result));

mysql_close($con);

