--TEST--
Test pinpoint
--INI--
pinpoint_agent.pinpoint_enable=true
pinpoint_agent.trace_exception=true
profiler.proxy.http.header.enable=true
pinpoint_agent.testCovered=1

--FILE--
<?php

class TestInterfaceInterceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;
    var $save_event;

    function __construct()
    {
        $this->apiId = pinpoint_add_api("Template::getHtml", 48); // functionName, lineno
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

        $p = new TestInterfaceInterceptor();
        $this->addInterceptor($p, "Template::getHtml", basename(__FILE__,".php"));

    }
}

$p = new QuickStartPlugin();
pinpoint_add_plugin($p, basename(__FILE__, '.php'));
pinpint_aop_reload();

interface iTemplate
{
    public function setVariable($name, $var);
    public function getHtml($template);
}

class Template implements iTemplate
{
    private $vars = array();

    public function setVariable($name, $var)
    {
        $this->vars[$name] = $var;
    }

    public function getHtml($template)
    {
        foreach($this->vars as $name => $value) {
            $template = str_replace('{' . $name . '}', $value, $template);
        }

        return $template;
    }
}

$template = new Template();

$template->setVariable("sam","lijin");

print $template->getHtml("hello {sam}");

?>
--EXPECTF--
%Srequest start
%SaddInterceptor name:[Template::getHtml] class:[test_interface]
%Scall Template::getHtml's interceptorPtr::onBefore
%SsetApiId:[%s]
%SsetServiceType:[1501]
%SaddAnnotation [-1]:[Array
%S(
%S[0] => hello {sam}
%S)
%S]
%Acall Template::getHtml's interceptorPtr::onEnd
%SaddAnnotation [14]:[hello lijin]
%Shello lijinrequest shutdown