--TEST--
Test pinpoint
--INI--
pinpoint_agent.trace_exception=true
pinpoint_agent.config_full_name=/home/liumingyi/git/pinpoint-c-agent/simulate/tmp/pinpoint_agent.conf
profiler.proxy.http.header.enable=true
pinpoint_agent.unittest=true
//pinpoint_agent.pluginsRootPath=php-plugins
//pinpoint_agent.entryFilename=plugins_create.php
--FILE--
<?php

class QuickStartPlugin extends \Pinpoint\Plugin
{
    public function __construct()
    {
        parent::__construct();
        $this->addSimpleInterceptor("hello", -1);
    }
}

$p = new QuickStartPlugin();
pinpoint_add_plugin($p, basename(__FILE__, '.php'));
pinpint_aop_reload();


function hello($who)
{
    echo "hello " .$who."\n";
    return $who;
}
hello("pinpoint");
?>
--EXPECTF--
request start
  addSimpleInterceptor name:hello
  call hello's interceptorPtr::onBefore
    SimpleInterceptor->addAnnotation key:-1 value:{[0]:pinpoint } 
hello pinpoint
  call hello's interceptorPtr::onEnd
    SimpleInterceptor->addAnnotation key:14 value:pinpoint 
request shutdown