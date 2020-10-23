<?php

namespace Plugins\AutoGen\workerman;

use Plugins\Util\Trace;

/**
 * Class HttpClientPlugin
 */
class HttpClientPlugin extends Candy
{
    protected $url;
    protected $option;
    protected $origin_success_cb;
    protected $origin_error_cb;
    protected $nsid;

    public function __construct($apId,$who,&...$args)
    {
        parent::__construct($apId,$who,$args);
        $this->url = $args[0];
        $this->option = &$args[1];

        $this->origin_success_cb = $args[1]['success'];
        $this->origin_error_cb = $args[1]['error'];
        $this->nsid = Trace::generateSpanID();
    }

    private function handleHttpHeader($url,&$headers)
    {
        if(pinpoint_get_context("Pinpoint-Sampled",$this->pinpoint_id) == PP_NOT_SAMPLED){
            $headers[] = 'Pinpoint-Sampled:s0';
            return ;
        }
        $headers[] ='Pinpoint-Sampled:s1';
        $headers[] ='Pinpoint-Flags:0';
        $headers[] ='Pinpoint-Papptype:1500';
        $headers[] ='Pinpoint-Pappname:'.APPLICATION_NAME;

        $headers[] = 'Pinpoint-Host:'.$url;

        $headers[] ='Pinpoint-Traceid:'.pinpoint_get_context(PP_TRANSCATION_ID,$this->pinpoint_id) ;
        $headers[] ='Pinpoint-Pspanid:'.pinpoint_get_context(PP_SPAN_ID,$this->pinpoint_id);

        $headers[] ='Pinpoint-Spanid:'.$this->nsid;
    }


    // @hook: Workerman\Http\Client::request
    function onBefore()
    {
        $this->startRequest();
        // start request
        $new_suc_cb =function ($reponse){
            $this->endRequest($reponse);
            if($this->origin_success_cb){
                ($this->origin_success_cb)($reponse);
            }

        };
        $this->option['success'] = $new_suc_cb;

        $new_fail_cb =function ($exception){
            $this->onError($exception);
            if($this->origin_error_cb){
                ($this->origin_error_cb)($exception);
            }
        };
        $this->option['error'] = $new_fail_cb;
        
        $headers = $this->option['headers'];
        $this->handleHttpHeader($this->url,$headers);
        $this->option['headers'] = $headers;
    }


    protected function startRequest()
    {
        echo $this->pinpoint_id." startRequest \n";
        pinpoint_add_clue(PP_DESTINATION,$this->getHostFromUrl($this->url),$this->pinpoint_id);
        pinpoint_add_clues(PP_PHP_ARGS,$this->url,$this->pinpoint_id);
        pinpoint_add_clue(PP_SERVER_TYPE,PP_PHP_REMOTE,$this->pinpoint_id);
        pinpoint_add_clue(PP_NEXT_SPAN_ID,$this->nsid,$this->pinpoint_id);
        pinpoint_add_clues(PP_HTTP_URL,$this->url,$this->pinpoint_id);
    }


    protected function onError($exception)
    {
        pinpoint_add_clue(PP_ADD_EXCEPTION,$exception,$this->pinpoint_id);
        pinpoint_add_clues(PP_HTTP_STATUS_CODE,0,$this->pinpoint_id);
        pinpoint_end_trace($this->pinpoint_id);
        echo $this->pinpoint_id." onError \n";
    }

    protected function endRequest($reponse)
    {
        $status_code = $reponse->getStatusCode();
        pinpoint_add_clues(PP_HTTP_STATUS_CODE,$status_code,$this->pinpoint_id);
        pinpoint_end_trace($this->pinpoint_id);
        echo $this->pinpoint_id." endRequest \n";
    }

    function onException($e)
    {

    }

    function onEnd(&$ret)
    {
        // do nothing
    }

    function getHostFromUrl($url)
    {
        $urlAr  = parse_url($url);
        return $urlAr['host'];
    }
}