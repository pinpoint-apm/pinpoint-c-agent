--TEST--
Test pinpoint
--INI--
pinpoint_agent.pinpoint_enable=true
pinpoint_agent.trace_exception=true
profiler.proxy.http.header.enable=true
pinpoint_agent.testCovered=1
;pinpoint.common.PPLogLevel = DEBUG
--FILE--
<?php

class QuickStartPlugin extends \Pinpoint\Plugin
{
    public function __construct()
    {
        parent::__construct();
        $this->addSimpleInterceptor("closure{test_closure.php:19}", -1);
    }
}

$p = new QuickStartPlugin();
pinpoint_add_plugin($p, basename(__FILE__, '.php'));
pinpint_aop_reload();


$greet = function($name)
{
    printf("Hello %s\r\n", $name);
};

class Simple{

    private $callback = null;

    public function run($input){

      // $($input);
      call_user_func($this->callback,$input);
   }
    public function __construct($cb)
    {
      $this->callback = $cb;
    }
};

$simple = new Simple($greet);
$simple->run('World');
$simple->run('PHP');

$greet("common");
?>

--EXPECTF--
request start
  addSimpleInterceptor name:[closure{test_closure.php:19}]
  call closure{test_closure.php:19}'s interceptorPtr::onBefore
    SimpleInterceptor->addAnnotation key:[-1] value:[{[0]:World }] 
Hello World
  call closure{test_closure.php:19}'s interceptorPtr::onEnd
    SimpleInterceptor->addAnnotation key:[14] value:[NULL] 
  call closure{test_closure.php:19}'s interceptorPtr::onBefore
    SimpleInterceptor->addAnnotation key:[-1] value:[{[0]:PHP }] 
Hello PHP
  call closure{test_closure.php:19}'s interceptorPtr::onEnd
    SimpleInterceptor->addAnnotation key:[14] value:[NULL] 
  call closure{test_closure.php:19}'s interceptorPtr::onBefore
    SimpleInterceptor->addAnnotation key:[-1] value:[{[0]:common }] 
Hello common
  call closure{test_closure.php:19}'s interceptorPtr::onEnd
    SimpleInterceptor->addAnnotation key:[14] value:[NULL] 

request shutdown