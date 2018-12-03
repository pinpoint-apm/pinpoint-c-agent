--TEST--
Test Function In Clone
--INI--
pinpoint_agent.pinpoint_enable=true
pinpoint_agent.trace_exception=true
profiler.proxy.http.header.enable=true
pinpoint_agent.testCovered=1

--FILE--

<?php

class TestCloneInterceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;
    var $save_event;
//    public function TestFunc1Interceptor()
    function __construct()
    {
        $this->apiId = pinpoint_add_api("Television::getColor", 32); // functionName, lineno
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

            $event->addAnnotation(PINPOINT_ANNOTATION_ARGS, print_r($args,true));


            $this->save_event = $event;
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
                $event->addAnnotation(PINPOINT_ANNOTATION_RETURN, htmlspecialchars(print_r($retArgs,true),ENT_QUOTES));
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
//                $trace->traceBlockEnd($event);
            }
        }
    }
}

class QuickStartPlugin extends Pinpoint\Plugin
{
    public function __construct(){
        parent::__construct();

        $p = new TestCloneInterceptor();
        $this->addInterceptor($p, "Television::getColor", basename(__FILE__, ".php"));
    }
}

$p = new QuickStartPlugin();
pinpoint_add_plugin($p, basename(__FILE__, '.php'));
pinpint_aop_reload();

class Television{
    protected $_color = 'black';
    protected $name = "ChangHong";

    public function setColor($color){
        $this->_color = $color;
    }

    /**
     * @return string
     */
    public function getColor()
    {
        return $this->name." color is ".$this->_color;
    }

    public function __clone()
    {
        $this->name = "ClonableName";
    }
}

$tv1 = new Television();
$tv1->setColor("red");
echo $tv1->getColor()."<br/>";

$tv2 = clone $tv1;
$tv2->setColor("white");
echo $tv2->getColor()."<br/>";
?>
--EXPECTF--
request start

  addInterceptor name:[Television::getColor] class:[test_clone]
  call Television::getColor's interceptorPtr::onBefore
    setApiId:[%i]
    setServiceType:[1501]
    addAnnotation [-1]:[Array
(
)
]
  call Television::getColor's interceptorPtr::onEnd
    addAnnotation [14]:[ChangHong color is red]
ChangHong color is red<br/>  call Television::getColor's interceptorPtr::onBefore
    setApiId:[%i]
    setServiceType:[1501]
    addAnnotation [-1]:[Array
(
)
]
  call Television::getColor's interceptorPtr::onEnd
    addAnnotation [14]:[ClonableName color is white]
ClonableName color is white<br/>request shutdown