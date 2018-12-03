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


class OnClientCloseInterceptor extends \Pinpoint\Interceptor
{
  var $apiId = -1;
  public function __construct()
  {
    $this->apiId = pinpoint_add_api("call_user_func", 10); // functionName, lineno
  }

  public function onBefore($callId, $args)
  {
    // $connection = $data["args"][0];

    // if(!isset($connection->traced))
    // {
    pinpoint_start_calltrace();
      // $connection->traced = 1;
    // }

    echo "-------------call---------------";
    $trace = pinpoint_get_current_trace();
    if ($trace)
    {
      $event = $trace->traceBlockBegin($callId);
      $event->markBeforeTime();
      $event->setApiId($this->apiId);
      $event->setServiceType(PINPOINT_PHP_RPC_TYPE);
      // $event->addAnnotation(PINPOINT_ANNOTATION_ARGS, "");
    }
  }

  public function onEnd($callId, $data)
  {
    $trace = pinpoint_get_current_trace();
    // $connection = $data["args"][0];
    
    if ($trace)
    {
      $retArgs = $data["result"];
      $event = $trace->getEvent($callId);
      if ($event)
      {
        // if ($retArgs)
        // {
          // $event->addAnnotation(PINPOINT_ANNOTATION_RETURN, htmlspecialchars(print_r($retArgs,true),ENT_QUOTES));
        // }
        $event->markAfterTime();
        $trace->traceBlockEnd($event);
      }
    }

    pinpoint_end_calltrace();
  }

  public function onException($callId, $exceptionStr)
  {

  }
}



class onClientMessageInterceptor extends \Pinpoint\Interceptor
{
  var $apiId = -1;
  public function __construct()
  {
    $this->apiId = pinpoint_add_api("closure{http_server.php:25}", 10); // functionName, lineno
  }

  public function onBefore($callId, $args)
  {
    pinpoint_start_calltrace();
    echo "-------------call---------------";
    $trace = pinpoint_get_current_trace();
    if ($trace)
    {
      $event = $trace->traceBlockBegin($callId);
      $event->markBeforeTime();
      $event->setApiId($this->apiId);
      $event->setServiceType(PINPOINT_PHP_RPC_TYPE);
      // $event->addAnnotation(PINPOINT_ANNOTATION_ARGS, "");
    }
  }

  public function onEnd($callId, $data)
  {
    $trace = pinpoint_get_current_trace();
    
    if ($trace)
    {
      $retArgs = $data["result"];
      $event = $trace->getEvent($callId);
      if ($event)
      {
        // if ($retArgs)
        // {
          // $event->addAnnotation(PINPOINT_ANNOTATION_RETURN, htmlspecialchars(print_r($retArgs,true),ENT_QUOTES));
        // }
        $event->markAfterTime();
        $trace->traceBlockEnd($event);
      }
    }

    pinpoint_end_calltrace();
  }

  public function onException($callId, $exceptionStr)
  {

  }
}



class WorkerManPlugins extends \Pinpoint\Plugin
{

    public function __construct()
    {
        parent::__construct();

        $i =  new OnClientCloseInterceptor();
        $this->addInterceptor($i,"closure{http_server.php:25}",basename(__FILE__));

        // $i =  new onClientMessageInterceptor();
        // $this->addInterceptor($i,"GatewayWorker\Gateway::onClientMessage",basename(__FILE__));


        $this->addSimpleInterceptor("Workerman\Connection\TcpConnection::send",-1);
        $this->addSimpleInterceptor("Workerman\Protocols\Http::encode",-1);
        // $this->addSimpleInterceptor("getmypid",-1);
        // $this->addSimpleInterceptor("GatewayWorker\Gateway::ping",-1);
    }
};

?>