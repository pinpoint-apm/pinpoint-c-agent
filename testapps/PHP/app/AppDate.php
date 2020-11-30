<?php


namespace app;


class AppDate
{
    static function outputDate(){
        echo date("Y/m/d")."<br>";
        echo getenv('REMOTE_ADDR')."<br>";
        echo getenv('HTTP_HOST')."<br>";
        echo getenv('HTTP_X_FORWARDED_FOR')."<br>";
        echo getenv('HTTP_X_FORWARDED_HOST')."<br>";
        echo getenv('HTTP_X_FORWARDED_SERVER')."<br>";
//        throw new \Exception("I want throw something");
//        print_r(ini_get_all('pinpoint_php'));
//        echo ini_get('pinpoint_php.CollectorHost')."\n";
//        echo ini_get('pinpoint_php.SendSpanTimeOutMs')."\n";
//        var_dump(ini_get('pinpoint_php.UnitTest"'));
//
//        echo ini_get('pinpoint_php.AppId')."\n";
//
//        echo ini_get('pinpoint_php.StartTime');
//        print_r($_SERVER);
    }
}