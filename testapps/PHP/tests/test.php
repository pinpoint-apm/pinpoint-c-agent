<?php
$dsn = 'mysql:host=127.0.0.1;port=3306';
$con = new PDO($dsn, "test","123456");
$sql = 'show databases;';
var_dump($con->query($sql));
