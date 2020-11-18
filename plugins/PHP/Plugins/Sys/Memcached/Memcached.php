<?php
namespace Plugins\Sys\Memcached;

class Memcached extends \Memcached
{
    public function add ($key, $value, $expiration = 0)
    {
        $p = new MemAddPlugin("Memcached::".__FUNCTION__,$this,$key,$value,$expiration);
        try{
            $p->onBefore();
            $r = parent::add($key, $value, $expiration);
            $p->onEnd($r);
            return $r;
        }catch (\Exception $e)
        {
            $p->onException($e);
            throw $e;
        }
    }

    public function addServer ($host, $port, $weight = 0)
    {
        $p = new MemAddServerPlugin("Memcached::".__FUNCTION__,$this,$host,$port,$weight);
        try{
            $p->onBefore();
            $r = parent::addServer($host,$port,$weight);
            $p->onEnd($r);
            return $r;
        }catch (\Exception $e)
        {
            $p->onException($e);
            throw $e;
        }
    }


    public function addServers ($servers)
    {
        $p = new MemAddServersPlugin("Memcached::".__FUNCTION__,$this,$servers);
        try{
            $p->onBefore();
            $r = parent::addServers($servers);
            $p->onEnd($r);
            return $r;
        }catch (\Exception $e)
        {
            $p->onException($e);
            throw $e;
        }
    }

    public function get($key, $cache_cb = null, $flags = 0)
    {
        $p = new MemGetPlugin("Memcached::".__FUNCTION__,$this,$key,$cache_cb,$flags);
        try{
            $p->onBefore();
            $r = parent::get($key,$cache_cb,$flags);
            $p->onEnd($r);
            return $r;
        }catch (\Exception $e)
        {
            $p->onException($e);
            throw $e;
        }
    }



}