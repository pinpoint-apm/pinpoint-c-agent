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

class Issue200F1Interceptor extends \Pinpoint\Interceptor
{
  var $apiId = -1;
  public function __construct()
  {
    $this->apiId = pinpoint_add_api("issue200_f1", -1); // functionName, lineno
    pinpoint_log(PINPOINT_INFO, sprintf("issue200_f1 api=%d", $this->apiId));
  }

  public function onBefore($callId, $args)
  {
    $trace = pinpoint_get_current_trace();
    if ($trace)
    {
      throw new Exception("xxx");
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
      }
    }
  }
}

class Issue200F2Interceptor extends \Pinpoint\Interceptor
{
  var $apiId = -1;
  public function __construct()
  {
    $this->apiId = pinpoint_add_api("issue200_f2", -1); // functionName, lineno
    pinpoint_log(PINPOINT_INFO, sprintf("issue200_f2 api=%d", $this->apiId));
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
    throw new Exception("onEnd throw");
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
      }
    }
  }
}

class Issue200F3Interceptor extends \Pinpoint\Interceptor
{
  var $apiId = -1;
  public function __construct()
  {
    $this->apiId = pinpoint_add_api("issue200_f3", -1); // functionName, lineno
    pinpoint_log(PINPOINT_INFO, sprintf("issue200_f3 api=%d", $this->apiId));
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
        $event->addAnnotation(PINPOINT_ANNOTATION_RETURN, htmlspecialchars(print_r($retArgs,true),ENT_QUOTES));
        $event->markAfterTime();
        $trace->traceBlockEnd($event);
      }
    }
  }

  public function onException($callId, $exceptionStr)
  {
    throw new Exception("onException throw");
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

class Issue200F4Interceptor extends \Pinpoint\Interceptor
{
  var $apiId = -1;
  public function __construct()
  {
    $this->apiId = pinpoint_add_api("issue200_f4", -1); // functionName, lineno
    pinpoint_log(PINPOINT_INFO, sprintf("issue200_f4 api=%d", $this->apiId));
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
    // forgot to call traceBlockEnd
  }

  public function onException($callId, $exceptionStr)
  {

  }
}

class Issue200Plugin extends \Pinpoint\Plugin
{

  public function __construct()
  {
    parent::__construct();

    $i = new Issue200F1Interceptor();
    $this->addInterceptor($i, "issue200_f1", "issue200_plugin.php");

    $i = new Issue200F2Interceptor();
    $this->addInterceptor($i, "issue200_f2", "issue200_plugin.php");

    $i = new Issue200F3Interceptor();
    $this->addInterceptor($i, "issue200_f3", "issue200_plugin.php");

    $i = new Issue200F4Interceptor();
    $this->addInterceptor($i, "issue200_f4", "issue200_plugin.php");

    $this->addSimpleInterceptor("issue200_f11", -1);
    $this->addSimpleInterceptor("issue200_f22", -1);
    $this->addSimpleInterceptor("issue200_f33", -1);
    $this->addSimpleInterceptor("issue200_f44", -1);
  }
}

?>