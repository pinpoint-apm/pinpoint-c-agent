<?php
namespace Plugins\Util;

class Trace{

    public static function generateSpanID()
    {
        try {
            return mt_rand();
        } catch (\Exception $e) {
            return rand();
        }
    }

}