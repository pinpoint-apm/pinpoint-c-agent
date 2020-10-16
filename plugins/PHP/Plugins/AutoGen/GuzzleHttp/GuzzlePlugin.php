<?php

namespace Plugins\AutoGen\GuzzleHttp;

use Plugins\Common\Candy;



/**
 *@hook:GuzzleHttp\ClientTrait::get
 *@hook:GuzzleHttp\ClientTrait::post
 *@hook:GuzzleHttp\ClientTrait::patch
 */
class GuzzlePlugin extends Candy
{
    function onBefore()
    {
        pinpoint_add_clue(DESTINATION,$this->getHostFromURL($this->args[0]));
        pinpoint_add_clues(HTTP_URL,$this->args[0]);
        pinpoint_add_clue(SERVER_TYPE,PINPOINT_PHP_REMOTE);

        $n_headers =[] ;
        if( is_array($this->args[1]) && array_key_exists('headers',$this->args[1]))
        {
            $n_headers = $this->args[1]['headers'];
        }
        $n_headers = array_merge($n_headers,$this->getNextSpanHeaders($this->args[0]));
        $this->args[1]['headers'] = $n_headers;
    }

    function onEnd(&$ret)
    {
//        $ret->getStatusCode();
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

        if(isset($urlAr['port']))
        {
            $retUrl .= ":".$urlAr['port'];
        }

//        if(isset($urlAr['path'])){
//            $retUrl.=$urlAr['path'];
//        }
//
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