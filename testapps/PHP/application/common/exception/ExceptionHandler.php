<?php
namespace app\common\exception;

use Exception;
use think\exception\Handle;
use think\exception\HttpException;
class ExceptionHandler extends Handle
{

    public function render(Exception $e)
    {
        // pinpoint_add_clue("EXP",$e->getMessage());
        pinpoint_mark_as_error($e->getMessage(), __FILE__);
        return parent::render($e);
    }

}