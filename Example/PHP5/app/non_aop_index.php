<?php
require_once __DIR__."/../vendor/autoload.php";

use app\NotWatchingAppDate;


$type = null;
if(isset($_GET["type"]))
{
    $type =  $_GET["type"];
}

switch ($type)
{
    case "test_get_data":
        NotWatchingAppDate::outputDate();
        break;
    default:
        echo "not found $type";
        break;
}



