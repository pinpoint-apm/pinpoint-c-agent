<?php


namespace app;


class TestGenerator
{
    function generator($start, $limit, $step=1){
        if($start > $limit){
            throw new LogicException("start cannot bigger that limit.");
        }
        usleep(120000);
        for($i = $start; $i<=$limit; $i += $step){
            try{
                yield $i;
            }catch (Exception $exception){
                echo $exception->getMessage();
            }
        }
    }
}