<?php

namespace Plugins;

use Plugins\Candy;

///@hook:GuzzleHttp\Client::request
class GuzzlePlugin extends Candy
{
    ///@hook:GuzzleHttp\Psr7\Request::__construct
    function onBefore()
    {
//        echo 'GuzzleHttp\Psr7\Request';
        if(strpos($this->apId, "Request::__construct") !== false){
            pinpoint_add_clue(DESTINATION,$this->getHostFromURL($this->args[1]));
            pinpoint_add_clues(HTTP_URL,$this->args[1]);
            pinpoint_add_clue(SERVER_TYPE,PINPOINT_PHP_REMOTE);

            $n_headers =[] ;
            if( !empty($this->args[2]))
            {
                $n_headers = $this->args[2];
            }

            $n_headers = array_merge($n_headers,$this->getNextSpanHeaders($this->args[1]));
            $this->args[2] = $n_headers;
        }
    }

    function onEnd(&$ret)
    {
        pinpoint_add_clues(HTTP_STATUS_CODE,(string)($ret->getStatusCode()));
    }

    function onException($e)
    {
        pinpoint_add_clue(EXCEPTION,$e->getMessage());
    }

    protected function getHostFromURL(string $url)
    {
        $urlAr   = parse_url($url);
        $retUrl = '';
        if(isset($urlAr['host']))
        {
            $retUrl.=$urlAr['host'];
        }

//        if(isset($urlAr['path'])){
//            $retUrl.=$urlAr['path'];
//        }

        if(isset($urlAr['port']))
        {
            $retUrl .= ":".$urlAr['port'];
        }
        return $retUrl;
    }

    protected function getNextSpanHeaders($host)
    {

        if(pinpoint_tracelimit()){
            $headers['Pinpoint-Sampled'] = 's0';
            return $headers;
        }

        $nsid = PerRequestPlugins::instance()->generateSpanID();
        $header = array(
            'Pinpoint-Sampled' =>'s1',
            'Pinpoint-Flags'=>0,
            'Pinpoint-Papptype'=>1500,
            'Pinpoint-Pappname'=>PerRequestPlugins::instance()->app_name,
            'Pinpoint-Host' =>$this->getHostFromURL($host),
            'Pinpoint-Traceid' =>PerRequestPlugins::instance()->tid,
            'Pinpoint-Pspanid'=>PerRequestPlugins::instance()->sid,
            'Pinpoint-Spanid'=>$nsid
            );

        return $header;
    }

}