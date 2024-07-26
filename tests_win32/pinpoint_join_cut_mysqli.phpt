--TEST--
pinpoint_php pinpoint_join_cut_mysqli
--SKIPIF--
<?php
if (!extension_loaded("pinpoint_php"))
  print "skip";
if(getenv('SKIP_WINDOWS_ACTION', true)){
  print "skip";
}  
?>
--INI--
pinpoint_php.DebugReport=true
--EXTENSIONS--
mysqli
--FILE--

<?php

$on_before = function (...$args) {
    $mysqli = pinpoint_get_this();
    $db_host = "localhost";
    if ($mysqli instanceof mysqli) {
        $db_host = $mysqli->host_info;
    }
    echo "$db_host \n";
};

$on_before_func = function (...$args) {
    $mysqli = $args[0];
    $db_host = "localhost";
    if ($mysqli instanceof mysqli) {
        $db_host = $mysqli->host_info;
    }
    echo "$db_host \n";
};

$on_end = function ($ret) {

};

$on_exception = function ($exp) {
};


_pinpoint_join_cut(['mysqli_query'], $on_before_func, $on_end, $on_exception);
_pinpoint_join_cut(['mysqli', 'query'], $on_before, $on_end, $on_exception);



mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);
$mysqli = new mysqli();

$mysqli->connect('dev-mysql', "root", "password", "employees", 3306);

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




--EXPECTF--
[pinpoint] [%d] [%d]try to interceptor function=mysqli_query
[pinpoint] [%d] [%d]added interceptor on `function`: mysqli_query success
[pinpoint] [%d] [%d]try to interceptor module(class)/function=mysqli:query
[pinpoint] [%d] [%d]added interceptor on `module`: mysqli::query success
[pinpoint] [%d] [%d]pinpoint_interceptor_handler_entry: handle func/method:mysqli::query
dev-mysql via TCP/IP 
[pinpoint] [%d] [%d] call_interceptor_before:mysqli::query return type(1) zval
[pinpoint] [%d] [%d]replace_ex_caller_parameters return value must be `an array`
[pinpoint] [%d] [%d]call_interceptor_end: mysqli::query 
Select returned 1000 rows.
[pinpoint] [%d] [%d]pinpoint_interceptor_handler_entry: handle func/method:mysqli_query
dev-mysql via TCP/IP 
[pinpoint] [%d] [%d] call_interceptor_before:mysqli_query return type(1) zval
[pinpoint] [%d] [%d]replace_ex_caller_parameters return value must be `an array`
[pinpoint] [%d] [%d]call_interceptor_end: mysqli_query 
[pinpoint] [%d] [%d]start free interceptor: mysqli_query
[pinpoint] [%d] [%d]start free interceptor: mysqli::query