--TEST--
Test Function In curl
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
                    $event->addAnnotation(PINPOINT_ANNOTATION_RETURN,sprintf("args:%s, return: url(%s) ",print_r($args,true),$retArgs["url"]));
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
/// add timeout 
curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 0); 
curl_setopt($ch, CURLOPT_TIMEOUT, 400); //timeout in seconds
curl_setopt($ch, CURLOPT_URL, $address);
curl_setopt($ch, CURLOPT_HTTPHEADER, array('setting:xiaozan'));

$output = curl_exec($ch);
$info = curl_getinfo($ch);
// disable this line, as the output is libcurl ddependency
// print_r($output);
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
    [1] =&gt; 78
    [2] =&gt; 0
)
]
  call curl_setopt's interceptorPtr::onEnd
    addAnnotation [14]:[args:Array
(
    [0] => Resource id #4
    [1] => 78
    [2] => 0
)
, return:1 ]
  call curl_setopt's interceptorPtr::onBefore
    setApiId:[%i]
    setServiceType:[1501]
    addAnnotation [-1]:[Array
(
    [0] =&gt; Resource id #4
    [1] =&gt; 13
    [2] =&gt; 400
)
]
  call curl_setopt's interceptorPtr::onEnd
    addAnnotation [14]:[args:Array
(
    [0] => Resource id #4
    [1] => 13
    [2] => 400
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
, return: url(http://abc.com/) ]
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