--TEST--
Test pinpoint
--INI--
pinpoint_agent.pinpoint_enable=true
pinpoint_agent.trace_exception=true
profiler.proxy.http.header.enable=true
pinpoint_agent.testCovered=1

--FILE--
<?php

class TestGeneratorInterceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;
    var $save_event;
    public function __construct()
    {
        $this->apiId = pinpoint_add_api("test_generator::generator", 2); // functionName, lineno
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


class QuickStartPlugin extends \Pinpoint\Plugin
{
    public function __construct()
    {
        parent::__construct();
        $p = new TestGeneratorInterceptor();
        $this->addInterceptor($p,"test_generator::generator",basename(__FILE__, '.php'));
    }
}

$p = new QuickStartPlugin();
pinpoint_add_plugin($p, basename(__FILE__, '.php'));
pinpint_aop_reload();

class test_generator{
    function generator($start, $limit, $step=1){
        if($start > $limit){
            throw new LogicException("start cannot bigger that limit.");
        }
        usleep(120000);
        for($i = $start; $i<=$limit; $i += $step){
            yield $i;
        }

    }
}
$test = new test_generator();
echo 'Single digit odd numbers from range(1,10):  ';
foreach ($test->generator(1, 10, 2) as $number) {
    echo "$number ";
}
?>

--EXPECTF--
request start
  addInterceptor name:[test_generator::generator] class:[test_generator]
Single digit odd numbers from range(1,10):    call test_generator::generator's interceptorPtr::onBefore
    setApiId:[%s]
    setServiceType:[1501]
    addAnnotation [-1]:[Array
(
    [0] =&gt; 1
    [1] =&gt; 10
    [2] =&gt; 2
)
]
  call test_generator::generator's interceptorPtr::onEnd
    addAnnotation [14]:[]
1   call test_generator::generator's interceptorPtr::onBefore
    setApiId:[-2]
    setServiceType:[1501]
    addAnnotation [-1]:[Array
(
    [0] =&gt; 1
    [1] =&gt; 10
    [2] =&gt; 2
)
]
  call test_generator::generator's interceptorPtr::onEnd
    addAnnotation [14]:[]
3   call test_generator::generator's interceptorPtr::onBefore
    setApiId:[-2]
    setServiceType:[1501]
    addAnnotation [-1]:[Array
(
    [0] =&gt; 1
    [1] =&gt; 10
    [2] =&gt; 2
)
]
  call test_generator::generator's interceptorPtr::onEnd
    addAnnotation [14]:[]
5   call test_generator::generator's interceptorPtr::onBefore
    setApiId:[-2]
    setServiceType:[1501]
    addAnnotation [-1]:[Array
(
    [0] =&gt; 1
    [1] =&gt; 10
    [2] =&gt; 2
)
]
  call test_generator::generator's interceptorPtr::onEnd
    addAnnotation [14]:[]
7   call test_generator::generator's interceptorPtr::onBefore
    setApiId:[-2]
    setServiceType:[1501]
    addAnnotation [-1]:[Array
(
    [0] =&gt; 1
    [1] =&gt; 10
    [2] =&gt; 2
)
]
  call test_generator::generator's interceptorPtr::onEnd
    addAnnotation [14]:[]
9   call test_generator::generator's interceptorPtr::onBefore
    setApiId:[-2]
    setServiceType:[1501]
    addAnnotation [-1]:[Array
(
    [0] =&gt; 1
    [1] =&gt; 10
    [2] =&gt; 2
)
]
  call test_generator::generator's interceptorPtr::onEnd
    addAnnotation [14]:[]

request shutdown