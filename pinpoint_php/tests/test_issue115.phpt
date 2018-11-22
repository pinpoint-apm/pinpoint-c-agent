--TEST--
Test pinpoint
--INI--
pinpoint_agent.pinpoint_enable=true
pinpoint_agent.trace_exception=true
profiler.proxy.http.header.enable=true
pinpoint_agent.testCovered=1

--FILE--
<?php
class TestISSUE115Interceptor extends \Pinpoint\Interceptor
{    var $apiId = -1;

    public function __construct()
    {
        $this->apiId = pinpoint_add_api("test_issue115::testFunc", 2); // functionName, lineno
    }

    public function onBefore($callId, $args)
    {
        $a = new test_issue115();
        echo $a->testFunc($a->num);
        $trace = pinpoint_get_current_trace();
        if ($trace)
        {
            $event = $trace->traceBlockBegin($callId);
            $event->markBeforeTime();
            $event->setApiId($this->apiId);
            $event->setServiceType(PINPOINT_PHP_RPC_TYPE);
            $event->addAnnotation(PINPOINT_ANNOTATION_ARGS,htmlspecialchars(print_r($args,true),ENT_QUOTES));

        }
    }

    public function onEnd($callId, $data)
    {
        $a = new test_issue115();
        echo $a->testFunc($a->num);
        $trace = pinpoint_get_current_trace();
        if ($trace)
        {
            $args = $data["args"];
            $retArgs = $data["result"];
            $event = $trace->getEvent($callId);
            if ($event)
            {
                $event->addAnnotation(PINPOINT_ANNOTATION_RETURN, htmlspecialchars(print_r($args,true),ENT_QUOTES));
                $event->markAfterTime();

                $trace->traceBlockEnd($event);
            }
        }
    }

    public function onException($callId, $exceptionStr)
    {
        $trace = pinpoint_get_current_trace();
        if ($trace)
        {
            $event = $trace->getEvent($callId);
            if ($event)
            {
                $event->markAfterTime();
                $event->setExceptionInfo($exceptionStr);
            }
        }
    }
}

class TestPlugin01 extends \Pinpoint\Plugin
{
    public function __construct()
    {
        parent::__construct();

        $p = new TestISSUE115Interceptor();
        $this->addInterceptor($p, "test_issue115::testFunc", basename(__FILE__, ".php"));
    }
}

$p = new TestPlugin01();
pinpoint_add_plugin($p, basename(__FILE__, '.php'));
pinpint_aop_reload();

class test_issue115
{
    public $num = 10;
    public $name = "sam";

    public function testFunc(&$arg){
        //        unset($arg);
        $arg = "bbbb";

        //throw new Exception("sssssssssssssss");
        echo "aaaaaaa";
        return $arg;
    }
}

$a = new test_issue115();
echo $a->testFunc($a->name);

?>
--EXPECTF--
request start
  addInterceptor name:[test_issue115::testFunc] class:[test_issue115]
  call test_issue115::testFunc's interceptorPtr::onBefore
aaaaaaabbbb    setApiId:[%i]
    setServiceType:[1501]
    addAnnotation [-1]:[Array
(
    [0] =&gt; sam
)
]
aaaaaaa  call test_issue115::testFunc's interceptorPtr::onEnd
aaaaaaabbbb    addAnnotation [14]:[Array
(
    [0] =&gt; bbbb
)
]
bbbbrequest shutdown