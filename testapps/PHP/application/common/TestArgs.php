<?php


namespace app\common;


class TestArgs
{
    public function  test_args($null_arg, $long_arg, $double_arg, $bool_arg, $array_arg, $object_arg, $object_tostring_arg, $string_arg, $resource_arg, $constant_arg){
        $tmp = $null_arg.'<br>'.$long_arg.'<br>'.$double_arg.'<br>'.$bool_arg.'<br>'.$object_tostring_arg.'<br>'.$string_arg.'<br>'.$constant_arg.'<br>';
        return sprintf("%s<br>%s<br>%s<br>%s",$tmp,implode($array_arg),var_dump($object_arg),var_dump($resource_arg));
    }
}