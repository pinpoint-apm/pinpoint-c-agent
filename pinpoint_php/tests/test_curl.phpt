--TEST--
Test pinpoint
--INI--
pinpoint_agent.pinpoint_enable=true
pinpoint_agent.trace_exception=true
profiler.proxy.http.header.enable=true
pinpoint_agent.testCovered=1
--SKIPIF--
<?php
if (!(in_array("curl",get_loaded_extensions()))) die("skip this test is for curl");
?>
--FILE--
<?php

class curlinitInterceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;
    public function __construct()
    {
        $this->apiId = pinpoint_add_api("curl_init", 10); // functionName, lineno
    }
    public function onBefore($callId, $args)
    {
        $trace = pinpoint_get_current_trace();
        if ($trace)
        {

            $event = $trace->traceBlockBegin($callId);
            $event->markBeforeTime();
            $event->setApiId($this->apiId);
            $event->setServiceType(PINPOINT_PHP_RPC_TYPE);
            $event->addAnnotation(PINPOINT_ANNOTATION_ARGS, htmlspecialchars(print_r($args,true),ENT_QUOTES));

        }
    }
    public function onEnd($callId, $data)
    {
        $trace = pinpoint_get_current_trace();
        if ($trace)
        {
            $args = $data["args"];
            $retArgs = $data["result"];
            $event = $trace->getEvent($callId);
            if ($event)
            {
                if ($retArgs)
                {
                    $event->addAnnotation(PINPOINT_ANNOTATION_RETURN,sprintf("args:%s, return:%s ",print_r($args,true),print_r($retArgs,true)));
                }
                $event->markAfterTime();
                $trace->traceBlockEnd($event);
            }
        }
    }

    public function onException($callId, $exceptionStr)
    {

    }
}

class curlsetoptInterceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;
    public function __construct()
    {
        $this->apiId = pinpoint_add_api("curl_setopt", 10); // functionName, lineno
    }
    public function onBefore($callId, $args)
    {
        $trace = pinpoint_get_current_trace();
        if ($trace)
        {

            $event = $trace->traceBlockBegin($callId);
            $event->markBeforeTime();
            $event->setApiId($this->apiId);
            $event->setServiceType(PINPOINT_PHP_RPC_TYPE);
            $event->addAnnotation(PINPOINT_ANNOTATION_ARGS, htmlspecialchars(print_r($args,true),ENT_QUOTES));

        }
    }
    public function onEnd($callId, $data)
    {
        $trace = pinpoint_get_current_trace();
        if ($trace)
        {
            $args = $data["args"];
            $retArgs = $data["result"];
            $event = $trace->getEvent($callId);
            if ($event)
            {
                if ($retArgs)
                {
                    $event->addAnnotation(PINPOINT_ANNOTATION_RETURN,sprintf("args:%s, return:%s ",print_r($args,true),print_r($retArgs,true)));
                }
                $event->markAfterTime();
                $trace->traceBlockEnd($event);
            }
        }
    }

    public function onException($callId, $exceptionStr)
    {

    }
}

class curlexecInterceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;
    public function __construct()
    {
        $this->apiId = pinpoint_add_api("curl_exec", 10); // functionName, lineno
    }
    public function onBefore($callId, $args)
    {
        $trace = pinpoint_get_current_trace();
        if ($trace)
        {

            $event = $trace->traceBlockBegin($callId);
            $event->markBeforeTime();
            $event->setApiId($this->apiId);
            $event->setServiceType(PINPOINT_PHP_RPC_TYPE);
            $event->addAnnotation(PINPOINT_ANNOTATION_ARGS, htmlspecialchars(print_r($args,true),ENT_QUOTES));

        }
    }
    public function onEnd($callId, $data)
    {
        $trace = pinpoint_get_current_trace();
        if ($trace)
        {
            $args = $data["args"];
            $retArgs = $data["result"];
            $event = $trace->getEvent($callId);
            if ($event)
            {
                if ($retArgs)
                {
                    $event->addAnnotation(PINPOINT_ANNOTATION_RETURN,sprintf("args:%s, return:%s ",print_r($args,true),print_r($retArgs,true)));
                }
                $event->markAfterTime();
                $trace->traceBlockEnd($event);
            }
        }
    }

    public function onException($callId, $exceptionStr)
    {

    }
}

class curlgetinfoInterceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;
    public function __construct()
    {
        $this->apiId = pinpoint_add_api("curl_getinfo", 10); // functionName, lineno
    }
    public function onBefore($callId, $args)
    {
        $trace = pinpoint_get_current_trace();
        if ($trace)
        {

            $event = $trace->traceBlockBegin($callId);
            $event->markBeforeTime();
            $event->setApiId($this->apiId);
            $event->setServiceType(PINPOINT_PHP_RPC_TYPE);
            $event->addAnnotation(PINPOINT_ANNOTATION_ARGS, htmlspecialchars(print_r($args,true),ENT_QUOTES));

        }
    }
    public function onEnd($callId, $data)
    {
        $trace = pinpoint_get_current_trace();
        if ($trace)
        {
            $args = $data["args"];
            $retArgs = $data["result"];
            $event = $trace->getEvent($callId);
            if ($event)
            {
                if ($retArgs)
                {
                    $event->addAnnotation(PINPOINT_ANNOTATION_RETURN,sprintf("args:%s, return:%s ",print_r($args,true),print_r($retArgs,true)));
                }
                $event->markAfterTime();
                $trace->traceBlockEnd($event);
            }
        }
    }

    public function onException($callId, $exceptionStr)
    {

    }
}

class curlcloseInterceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;
    public function __construct()
    {
        $this->apiId = pinpoint_add_api("curl_close", 10); // functionName, lineno
    }
    public function onBefore($callId, $args)
    {
        $trace = pinpoint_get_current_trace();
        if ($trace)
        {

            $event = $trace->traceBlockBegin($callId);
            $event->markBeforeTime();
            $event->setApiId($this->apiId);
            $event->setServiceType(PINPOINT_PHP_RPC_TYPE);
            $event->addAnnotation(PINPOINT_ANNOTATION_ARGS, htmlspecialchars(print_r($args,true),ENT_QUOTES));

        }
    }
    public function onEnd($callId, $data)
    {
        $trace = pinpoint_get_current_trace();
        if ($trace)
        {
            $args = $data["args"];
            $retArgs = $data["result"];
            $event = $trace->getEvent($callId);
            if ($event)
            {
                if ($retArgs)
                {
                    $event->addAnnotation(PINPOINT_ANNOTATION_RETURN,sprintf("args:%s, return:%s ",print_r($args,true),print_r($retArgs,true)));
                }
                $event->markAfterTime();
                $trace->traceBlockEnd($event);
            }
        }
    }

    public function onException($callId, $exceptionStr)
    {

    }
}

class QuickStartPlugin extends \Pinpoint\Plugin
{
    public function __construct()
    {
        parent::__construct();
        $p = new curlinitInterceptor();
        $this->addInterceptor($p,"curl_init",basename(__FILE__, '.php'));

        $p = new curlsetoptInterceptor();
        $this->addInterceptor($p,"curl_setopt",basename(__FILE__, '.php'));

        $p = new curlexecInterceptor();
        $this->addInterceptor($p,"curl_exec",basename(__FILE__, '.php'));

        $p = new curlgetinfoInterceptor();
        $this->addInterceptor($p,"curl_getinfo",basename(__FILE__, '.php'));

        $p = new curlcloseInterceptor();
        $this->addInterceptor($p,"curl_close",basename(__FILE__, '.php'));
    }
}

$p = new QuickStartPlugin();
pinpoint_add_plugin($p, basename(__FILE__, '.php'));
pinpint_aop_reload();

$address = "http://abc.com";
$ch = curl_init();
curl_setopt($ch, CURLOPT_HEADER, 0);
curl_setopt($ch, CURLOPT_NOBODY, 0);
curl_setopt($ch, CURLOPT_URL, $address);
curl_setopt($ch, CURLOPT_HTTPHEADER, array('setting:xiaozan'));

$output = curl_exec($ch);
$info = curl_getinfo($ch);
print_r($output);
curl_close($ch);

?>

--EXPECTF--
request start
  addInterceptor name:[curl_init] class:[test_curl]
  addInterceptor name:[curl_setopt] class:[test_curl]
  addInterceptor name:[curl_exec] class:[test_curl]
  addInterceptor name:[curl_getinfo] class:[test_curl]
  addInterceptor name:[curl_close] class:[test_curl]
  call curl_init's interceptorPtr::onBefore
    setApiId:[%i]
    setServiceType:[1501]
    addAnnotation [-1]:[Array
(
)
]
  call curl_init's interceptorPtr::onEnd
    addAnnotation [14]:[args:Array
(
)
, return:Resource id #4 ]
  call curl_setopt's interceptorPtr::onBefore
    setApiId:[%i]
    setServiceType:[1501]
    addAnnotation [-1]:[Array
(
    [0] =&gt; Resource id #4
    [1] =&gt; 42
    [2] =&gt; 0
)
]
  call curl_setopt's interceptorPtr::onEnd
    addAnnotation [14]:[args:Array
(
    [0] => Resource id #4
    [1] => 42
    [2] => 0
)
, return:1 ]
  call curl_setopt's interceptorPtr::onBefore
    setApiId:[%i]
    setServiceType:[1501]
    addAnnotation [-1]:[Array
(
    [0] =&gt; Resource id #4
    [1] =&gt; 44
    [2] =&gt; 0
)
]
  call curl_setopt's interceptorPtr::onEnd
    addAnnotation [14]:[args:Array
(
    [0] => Resource id #4
    [1] => 44
    [2] => 0
)
, return:1 ]
  call curl_setopt's interceptorPtr::onBefore
    setApiId:[%i]
    setServiceType:[1501]
    addAnnotation [-1]:[Array
(
    [0] =&gt; Resource id #4
    [1] =&gt; 10002
    [2] =&gt; http://abc.com
)
]
  call curl_setopt's interceptorPtr::onEnd
    addAnnotation [14]:[args:Array
(
    [0] => Resource id #4
    [1] => 10002
    [2] => http://abc.com
)
, return:1 ]
  call curl_setopt's interceptorPtr::onBefore
    setApiId:[%i]
    setServiceType:[1501]
    addAnnotation [-1]:[Array
(
    [0] =&gt; Resource id #4
    [1] =&gt; 10023
    [2] =&gt; Array
        (
            [0] =&gt; setting:xiaozan
        )

)
]
  call curl_setopt's interceptorPtr::onEnd
    addAnnotation [14]:[args:Array
(
    [0] => Resource id #4
    [1] => 10023
    [2] => Array
        (
            [0] => setting:xiaozan
        )

)
, return:1 ]
  call curl_exec's interceptorPtr::onBefore
    setApiId:[%i]
    setServiceType:[1501]
    addAnnotation [-1]:[Array
(
    [0] =&gt; Resource id #4
)
]
  call curl_exec's interceptorPtr::onEnd
  call curl_getinfo's interceptorPtr::onBefore
    setApiId:[%i]
    setServiceType:[1501]
    addAnnotation [-1]:[Array
(
    [0] =&gt; Resource id #4
)
]
  call curl_getinfo's interceptorPtr::onEnd
    addAnnotation [14]:[args:Array
(
    [0] => Resource id #4
)
, return:Array
(
    [url] => http://abc.com/
    [content_type] => 
    [http_code] => 0
    [header_size] => 0
    [request_size] => %d
    [filetime] => %s
    [ssl_verify_result] => 0
    [redirect_count] => 0
    [total_time] => %f
    [namelookup_time] => %f
    [connect_time] => %f
    [pretransfer_time] => %f
    [size_upload] => 0
    [size_download] => 0
    [speed_download] => 0
    [speed_upload] => 0
    [download_content_length] => %i
    [upload_content_length] => %i
    [starttransfer_time] => 0
    [redirect_time] => 0
    [redirect_url] => 
    [primary_ip] => %s
    [certinfo] => Array
        (
        )

    [primary_port] => %d
    [local_ip] => %s
    [local_port] => %d
)
 ]
  call curl_close's interceptorPtr::onBefore
    setApiId:[%i]
    setServiceType:[1501]
    addAnnotation [-1]:[Array
(
    [0] =&gt; Resource id #4
)
]
  call curl_close's interceptorPtr::onEnd

request shutdown