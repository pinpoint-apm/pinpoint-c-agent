<?php
/**
 * Copyright 2018 NAVER Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

class GetDateInterceptor extends \Pinpoint\Interceptor
{
  var $apiId = -1;
  public function __construct()
  {
    $this->apiId = pinpoint_add_api("date", 10); // functionName, lineno
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
      $self = $this->getSelf();
      if ($self)
      {
        $event->addAnnotation(PINPOINT_ANNOTATION_ARGS,
            sprintf("[ %s ] \n this.num=%d ",htmlspecialchars(print_r($args,true),ENT_QUOTES),$self->num));
      }else{
        $event->addAnnotation(PINPOINT_ANNOTATION_ARGS, htmlspecialchars(print_r($args,true),ENT_QUOTES));
      }

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
          $event->addAnnotation(PINPOINT_ANNOTATION_RETURN, htmlspecialchars(print_r($retArgs,true),ENT_QUOTES));
        }
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

class TestFunc1Interceptor extends \Pinpoint\Interceptor
{
  var $apiId = -1;
  public function __construct()
  {
    $this->apiId = pinpoint_add_api("test_func1", 10); // functionName, lineno
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
      $self = $this->getSelf();
      if ($self)
      {
        $event->addAnnotation(PINPOINT_ANNOTATION_ARGS,
            sprintf("[ %s ] \n this.num=%d ",htmlspecialchars(print_r($args,true),ENT_QUOTES),$self->num));
      }else{
        $event->addAnnotation(PINPOINT_ANNOTATION_ARGS, htmlspecialchars(print_r($args,true),ENT_QUOTES));
      }

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
          $event->addAnnotation(PINPOINT_ANNOTATION_RETURN, htmlspecialchars(print_r($retArgs,true),ENT_QUOTES));
        }
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



class TestFunc1NULLInterceptor extends \Pinpoint\Interceptor
{
  var $apiId = -1;
  public function __construct()
  {
    $this->apiId = pinpoint_add_api("test_null", 10); // functionName, lineno
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
      //
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
          $event->addAnnotation(PINPOINT_ANNOTATION_RETURN, htmlspecialchars(print_r($retArgs,true),ENT_QUOTES));
        }
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

class TestFunc2Interceptor extends \Pinpoint\Interceptor
{
  var $apiId = -1;
  public function __construct()
  {
    $this->apiId = pinpoint_add_api("testNameSpace\\test_func2", 10); // functionName, lineno
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
      $self = $this->getSelf();
      if ($self)
      {
        $event->addAnnotation(PINPOINT_ANNOTATION_ARGS,
            sprintf("[ %s ] \n this.num=%d ",htmlspecialchars(print_r($args,true),ENT_QUOTES),$self->num));
      }else{
        $event->addAnnotation(PINPOINT_ANNOTATION_ARGS, htmlspecialchars(print_r($args,true),ENT_QUOTES));
      }

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
          $event->addAnnotation(PINPOINT_ANNOTATION_RETURN, htmlspecialchars(print_r($retArgs,true),ENT_QUOTES));
        }
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

class TestMethodInterceptor extends \Pinpoint\Interceptor
{
  var $apiId = -1;
  public function __construct()
  {
    $this->apiId = pinpoint_add_api("test_method", 10); // functionName, lineno
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
      $self = $this->getSelf();
      if ($self)
      {
        $event->addAnnotation(PINPOINT_ANNOTATION_ARGS,
            sprintf("[ %s ] \n this.num=%d ",htmlspecialchars(print_r($args,true),ENT_QUOTES),$self->num));
      }else{
        $event->addAnnotation(PINPOINT_ANNOTATION_ARGS, htmlspecialchars(print_r($args,true),ENT_QUOTES));
      }

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
          $event->addAnnotation(PINPOINT_ANNOTATION_RETURN, htmlspecialchars(print_r($retArgs,true),ENT_QUOTES));
        }
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

class TestCumsum1Interceptor extends \Pinpoint\Interceptor
{
  var $apiId = -1;
  public function __construct()
  {
    $this->apiId = pinpoint_add_api("test_cumsum1", -1); // functionName, lineno
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
          $event->addAnnotation(PINPOINT_ANNOTATION_RETURN, htmlspecialchars(print_r($retArgs,true),ENT_QUOTES));
        }
        $event->markAfterTime();
        $trace->traceBlockEnd($event);
      }
    }
  }

  public function onException($callId, $exceptionStr)
  {
    pinpoint_log(PINPOINT_INFO, "call TestExceptionInterceptor onException.");
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

class TestCumsum3Interceptor extends \Pinpoint\Interceptor
{
  var $apiId = -1;
  public function __construct()
  {
    $this->apiId = pinpoint_add_api("test_cumsum3", -1); // functionName, lineno
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
      //$retArgs = $data["result"];
      $retArgs = NULL;
      $event = $trace->getEvent($callId);
      if ($event)
      {
        if ($retArgs)
        {
          $event->addAnnotation(PINPOINT_ANNOTATION_RETURN, htmlspecialchars(print_r($retArgs,true),ENT_QUOTES));
        }
        $event->markAfterTime();
        $trace->traceBlockEnd($event);
      }
    }
  }

  public function onException($callId, $exceptionStr)
  {
    pinpoint_log(PINPOINT_INFO, "call TestExceptionInterceptor onException.");
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

class TestCumsumE1Interceptor extends \Pinpoint\Interceptor
{
  var $apiId = -1;
  public function __construct()
  {
    $this->apiId = pinpoint_add_api("test_cumsum_e1", -1); // functionName, lineno
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
          $event->addAnnotation(PINPOINT_ANNOTATION_RETURN, htmlspecialchars(print_r($retArgs,true),ENT_QUOTES));
        }
        $event->markAfterTime();
        $trace->traceBlockEnd($event);
      }
    }
  }

  public function onException($callId, $exceptionStr)
  {
    pinpoint_log(PINPOINT_INFO, "call TestExceptionInterceptor onException.");

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


class QuickStartPlugin extends \Pinpoint\Plugin
{

  public function __construct()
  {
    // you have to call the construct of parent class here
    pinpoint_log(PINPOINT_DEBUG, "__construct");
    parent::__construct();
  pinpoint_log(PINPOINT_DEBUG, "after __construct");
    $this->addSimpleInterceptor("Redis::connect", -1);
    $this->addSimpleInterceptor("test_simple_interceptor", -1);
    $this->addSimpleInterceptor("Redis::set", -1);
    $this->addSimpleInterceptor("Redis::get", -1);

    $this->addSimpleInterceptor("Redis::delete", -1);
    $this->addSimpleInterceptor("Redis::keys", -1);

    $i = new TestMethodInterceptor();
    $this->addInterceptor($i, "testNameSpace\\TestClass::getVarAdd", "quickstart_plugin.php");

    $i = new TestCumsum1Interceptor();
    $this->addInterceptor($i, "test_cumsum1", "quickstart_plugin.php");

    $i = new TestCumsum3Interceptor();
    $this->addInterceptor($i, "test_cumsum3", "quickstart_plugin.php");

    $i = new TestCumsumE1Interceptor();
    $this->addInterceptor($i, "test_cumsum_e1", "quickstart_plugin.php");

    $i = new GetDateInterceptor();
    $this->addInterceptor($i, "date", "quickstart_plugin.php");

    $i = new TestFunc1Interceptor();
    $this->addInterceptor($i, "test_func1", "quickstart_plugin.php");

    $i = new TestFunc1NULLInterceptor();
    $this->addInterceptor($i, "test_null", "quickstart_plugin.php");

    $i = new TestFunc2Interceptor();
    $this->addInterceptor($i, "testNameSpace\\test_func2", "quickstart_plugin.php");

    $this->addSimpleInterceptor("test_cumsum2", -1);
    $this->addSimpleInterceptor("test_cumsum4", -1);
    $this->addSimpleInterceptor("test_cumsum_e2", -1);
    $this->addSimpleInterceptor("test_exception3", -1);
    pinpoint_log(PINPOINT_DEBUG, "quickstart plugin works");

  }
};
?>