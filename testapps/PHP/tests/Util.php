<?php


namespace tests;

$errors= ["PHP Warning", "Fatal error", "Deprecated", "Declaration"];

class Util
{
    public function get_log(){
        $logfile = fopen(PHP_LOG_PATH, "r") or die("Unable to open file!");
        $log = fread($logfile, filesize(PHP_LOG_PATH));
        fclose($logfile);
        return $log;
    }

    public function check_error($str){
        $errors= ["PHP Warning", "Fatal error", "Deprecated", "Declaration"];
        foreach($errors as $err){
            if(strpos($str, $err) !== false){
                return false;
            }
        }
        return true;
    }

    public function check_span($span, $values=[]){
        if(count($values)!==0){
            foreach($values as $value){
                if(strpos($span, $value) == false){
                    return false;
                }
            }
        }
        return true;
    }

}