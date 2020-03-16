<?php


namespace app;


class TestArgs
{
    public function  test_args($null_arg, $long_arg, $double_arg, $bool_arg, $array_arg, $object_arg, $object_tostring_arg, $string_arg, $resource_arg, $constant_arg){
        echo $null_arg.'<br>', $long_arg.'<br>', $double_arg.'<br>', $bool_arg.'<br>', $object_tostring_arg.'<br>', $string_arg.'<br>', $constant_arg.'<br>';
        var_dump($array_arg);
        echo '<br>';
        var_dump($object_arg);
        echo '<br>';
        var_dump($resource_arg);
        echo '<br>';
    }
}