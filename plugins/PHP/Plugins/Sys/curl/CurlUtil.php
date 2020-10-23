<?php


namespace Plugins\Sys\curl;

use Plugins\Framework\Swoole\IDContext;
use Plugins\Util\Trace;

class CurlUtil
{
	public static function curlPinpointHeader($ch, &$headers)
    {
        if(PerRequestPlugins::instance()->traceLimit()){
            $headers[] = 'Pinpoint-Sampled:s0';
            return ;
        }

        $headers[] ='Pinpoint-Sampled:s1';
        $headers[] ='Pinpoint-Flags:0';
        $headers[] ='Pinpoint-Papptype:1500';
        $headers[] ='Pinpoint-Pappname:'.PerRequestPlugins::instance()->app_name;

        $headers[] = 'Pinpoint-Host:'.static::getHostFromURL(curl_getinfo($ch,CURLINFO_EFFECTIVE_URL));

        $headers[] ='Pinpoint-Traceid:'.PerRequestPlugins::instance()->tid;
        $headers[] ='Pinpoint-Pspanid:'.PerRequestPlugins::instance()->sid;
        $nsid = PerRequestPlugins::instance()->generateSpanID();
        $headers[] ='Pinpoint-Spanid:'.$nsid;
    }

    public static function appendPinpointHeader($url, &$headers)
    {
        if(pinpoint_get_context('Pinpoint-Sampled', IDContext::get())=="s0"){
            $headers[] = 'Pinpoint-Sampled:s0';
            return ;
        }

        $headers[] ='Pinpoint-Sampled:s1';
        $headers[] ='Pinpoint-Flags:0';
        $headers[] ='Pinpoint-Papptype:1500';
        $headers[] ='Pinpoint-Pappname:'.APPLICATION_NAME;

        $headers[] = 'Pinpoint-Host:'.static::getHostFromURL($url);

        $headers[] ='Pinpoint-Traceid:'.pinpoint_get_context('tid', IDContext::get());
        $headers[] ='Pinpoint-Pspanid:'.pinpoint_get_context('sid', IDContext::get());
        $nsid = Trace::generateSpanID();
        $headers[] ='Pinpoint-Spanid:'.$nsid;
        pinpoint_set_context('nsid', (string)$nsid, IDContext::get());
    }

    public static function appendPinpointHeaderInKeyValue($url, &$headers)
    {
        if(pinpoint_get_context('Pinpoint-Sampled', IDContext::get())=="s0"){
            $headers['Pinpoint-Sampled'] ='s0';
            return ;
        }

        $headers['Pinpoint-Sampled'] ='s1';
        $headers['Pinpoint-Flags'] ='0';
        $headers['Pinpoint-Papptype'] ='1500';
        $headers['Pinpoint-Pappname'] = APPLICATION_NAME;

        $headers['Pinpoint-Host'] = static::getHostFromURL($url);

        $headers['Pinpoint-Traceid'] = pinpoint_get_context('tid', IDContext::get());
        $headers['Pinpoint-Pspanid'] = pinpoint_get_context('sid', IDContext::get());
        $nsid = Trace::generateSpanID();
        $headers['Pinpoint-Spanid'] = $nsid;
        pinpoint_set_context('nsid', (string)$nsid, IDContext::get());
    }



    public static function addPinpointHeader($ch,$url)
    {
        if(PerRequestPlugins::instance()->traceLimit()){
            \curl_setopt($ch,CURLOPT_HTTPHEADER,array("Pinpoint-Sampled:s0"));
            return ;
        }

        $nsid = PerRequestPlugins::instance()->generateSpanID();
        $header = array(
            'Pinpoint-Sampled:s1',
            'Pinpoint-Flags:0',
            'Pinpoint-Papptype:1500',
            'Pinpoint-Pappname:'.PerRequestPlugins::instance()->app_name,
            'Pinpoint-Host:'.static::getHostFromURL($url),
            'Pinpoint-Traceid:'.PerRequestPlugins::instance()->tid,
            'Pinpoint-Pspanid:'.PerRequestPlugins::instance()->sid,
            'Pinpoint-Spanid:'.$nsid
        );
        \curl_setopt($ch,CURLOPT_HTTPHEADER,$header);
    }

    public static function getHostFromURL(string $url)
    {
        $urlAr   = parse_url($url);
        $retUrl = '';
        if(isset($urlAr['host']))
        {
            $retUrl.=$urlAr['host'];
        }
// note disable path
//        if(isset($urlAr['path'])){
//            $retUrl.=$urlAr['path'];
//        }

        if(isset($urlAr['port']))
        {
            $retUrl .= ":".$urlAr['port'];
        }

        return $retUrl;
    }

}