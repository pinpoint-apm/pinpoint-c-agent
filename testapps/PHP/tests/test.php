<?php
$dsn = 'mysql:host=localhost;port=3306';
$con = new PDO($dsn, "root","123456");
$sql = 'show databases;';
var_dump($con->query($sql));
