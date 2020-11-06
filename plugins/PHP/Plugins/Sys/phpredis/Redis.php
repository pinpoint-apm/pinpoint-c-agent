<?php
/*
 * User: eeliu
 * Date: 11/5/20
 * Time: 4:17 PM
 */

namespace Plugins\Sys\phpredis;


class Redis extends \Redis
{
    public function connect( $host, $port = 6379, $timeout = 0.0, $reserved = null, $retry_interval = 0 )
    {
        $p = new ConnectPlugin("Redis::".__FUNCTION__,$this,$host, $port, $timeout, $reserved, $retry_interval);
        try{
            $p->onBefore();
            $r = parent::connect($host, $port, $timeout, $reserved, $retry_interval);
            $p->onEnd($r);
            return $r;
        }catch (\Exception $e)
        {
            $p->onException($e);
            throw $e;
        }

    }

    public function get( $key )
    {
        $p = new GetPlugin("Redis::".__FUNCTION__,$this,$key);
        try{
            $p->onBefore();
            $r = parent::get($key);
            $p->onEnd($r);
            return $r;
        }catch (\Exception $e)
        {
            $p->onException($e);
            throw $e;
        }
    }

    public function set($key, $value, $timeout = null)
    {

        $p = new SetPlugin("Redis::".__FUNCTION__,$this,$key, $value, $timeout);
        try{
            $p->onBefore();
            $r = parent::set($key, $value, $timeout);
            $p->onEnd($r);
            return $r;
        }catch (\Exception $e)
        {
            $p->onException($e);
            throw $e;
        }

    }

}