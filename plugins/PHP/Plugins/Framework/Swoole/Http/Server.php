<?php

namespace Plugins\Framework\Swoole\Http;
use Swoole\Http\Server as SServer;

class Server extends SServer
{
    public function on($event,callable $callback)
    {
        switch ($event)
        {
            case 'request':
                $callback = new PerReqPlugin($callback);
                break;
        }
        parent::on($event,$callback);

    }

}