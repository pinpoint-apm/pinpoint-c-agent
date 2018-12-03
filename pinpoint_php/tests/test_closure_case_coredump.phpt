--TEST--
Test Closure Function
--INI--
pinpoint_agent.pinpoint_enable=true
pinpoint_agent.trace_exception=true
profiler.proxy.http.header.enable=true
pinpoint_agent.testCovered=1
;pinpoint.common.PPLogLevel = DEBUG
--SKIPIF--
<?php
if (substr(phpversion(), 0, 1) != '7') die("skip this test is for php 7");
?>
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

    private $callback;

    public function run($input){

      $callback($input);

   }
    public function __construct( $cb)
    {
      $callback = $cb;
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
[ERROR] file:[%s] line:[%d] msg:[Undefined variable: callback]

Notice: Undefined variable: callback in %s on line %d
[EXCEPTION] file:[%s] line:[%d] msg:[Function name must be a string]

Fatal error: Uncaught Error: Function name must be a string in %s:%d
Stack trace:
#0 %s(38): Simple->run('World')
#1 {main}
  thrown in %s on line %d
request shutdown