<?php
/**
 * Created by PhpStorm.
 * User: test
 * Date: 6/23/20
 * Time: 3:12 PM
 */

namespace Plugins;

use Plugins\Candy;

///@hook:GuzzleHttp\Client::request
class GuzzlePlugin extends Candy
{
    ///@hook:GuzzleHttp\Psr7\Request::__construct
    function onBefore()
    {
        if(strpos($this->apId, "Request::__construct")){

            pinpoint_add_clue(DESTINATION,$this->getHostFromURL($this->args[1]));
            pinpoint_add_clues(HTTP_URL,$this->args[1]);
            pinpoint_add_clue(SERVER_TYPE,PINPOINT_PHP_REMOTE);

        }else{
        }

        // TODO: Implement onBefore() method.
    }

    function onEnd(&$ret)
    {
        pinpoint_add_clues(HTTP_STATUS_CODE,(string)($ret->getStatusCode()));
    }

    function onException($e)
    {
        pinpoint_add_clue(EXCEPTION,$e->getMessage());
    }

    function getHostFromURL(string $url)
    {
        $urlAr   = parse_url($url);
        $retUrl = '';
        if(isset($urlAr['host']))
        {
            $retUrl.=$urlAr['host'];
        }

        if(isset($urlAr['path'])){
            $retUrl.=$urlAr['path'];
        }

        if(isset($urlAr['port']))
        {
            $retUrl .= ":".$urlAr['port'];
        }

        return $retUrl;
    }
}