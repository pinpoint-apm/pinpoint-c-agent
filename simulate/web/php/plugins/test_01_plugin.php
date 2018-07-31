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

class TestOnEnd01Interceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;
    public function __construct()
    {
        $this->apiId = pinpoint_add_api("test_onEnd1::testFunc", 2); // functionName, lineno
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

            pinpoint_log(PINPOINT_INFO, "PINPOINT_INFO-----");

            $event->addAnnotation(PINPOINT_ANNOTATION_ARGS,htmlspecialchars(print_r($args,true),ENT_QUOTES));
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
                $event->addAnnotation(PINPOINT_ANNOTATION_RETURN, htmlspecialchars(print_r($retArgs,true),ENT_QUOTES));

                $event->markAfterTime();
                $trace->traceBlockEnd($event);
            }
            pinpoint_log(PINPOINT_ERROR, "PINPOINT_ERROR-----");
        }
    }

    public function onException($callId, $exceptionStr)
    {
        $trace = pinpoint_get_current_trace();
        pinpoint_log(PINPOINT_DEBUG, "PINPOINT_DEBUG-----");
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

class TestOnEnd02Interceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;
    public function __construct()
    {
        $this->apiId = pinpoint_add_api("test_onEnd2::testFunc", 2); // functionName, lineno
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

            pinpoint_log(PINPOINT_INFO, "PINPOINT_INFO-----");

            $event->addAnnotation(PINPOINT_ANNOTATION_ARGS,htmlspecialchars(print_r($args,true),ENT_QUOTES));
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
                $event->addAnnotation(PINPOINT_ANNOTATION_RETURN, htmlspecialchars(print_r($retArgs,true),ENT_QUOTES));

                $event->markAfterTime();
                $trace->traceBlockEnd($event);
            }
            pinpoint_log(PINPOINT_ERROR, "PINPOINT_ERROR-----");
        }
    }

    public function onException($callId, $exceptionStr)
    {
        $trace = pinpoint_get_current_trace();
        pinpoint_log(PINPOINT_DEBUG, "PINPOINT_DEBUG-----");
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

class TestOnEnd03Interceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;
    public function __construct()
    {
        $this->apiId = pinpoint_add_api("test_onEnd3::testFunc", 2); // functionName, lineno
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

            pinpoint_log(PINPOINT_INFO, "PINPOINT_INFO-----");

            $event->addAnnotation(PINPOINT_ANNOTATION_ARGS,htmlspecialchars(print_r($args,true),ENT_QUOTES));
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
                $event->addAnnotation(PINPOINT_ANNOTATION_RETURN, htmlspecialchars(print_r($retArgs,true),ENT_QUOTES));

                $event->markAfterTime();
                $trace->traceBlockEnd($event);
            }
            pinpoint_log(PINPOINT_ERROR, "PINPOINT_ERROR-----");
        }
    }

    public function onException($callId, $exceptionStr)
    {
        $trace = pinpoint_get_current_trace();
        pinpoint_log(PINPOINT_DEBUG, "PINPOINT_DEBUG-----");
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

class TestOnEnd04Interceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;
    public function __construct()
    {
        $this->apiId = pinpoint_add_api("test_onEnd4::testFunc", 2); // functionName, lineno
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

            pinpoint_log(PINPOINT_INFO, "PINPOINT_INFO-----");

            $event->addAnnotation(PINPOINT_ANNOTATION_ARGS,htmlspecialchars(print_r($args,true),ENT_QUOTES));
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
                $event->addAnnotation(PINPOINT_ANNOTATION_RETURN, htmlspecialchars(print_r($retArgs,true),ENT_QUOTES));

                $event->markAfterTime();
                $trace->traceBlockEnd($event);
            }
            pinpoint_log(PINPOINT_ERROR, "PINPOINT_ERROR-----");
        }
    }

    public function onException($callId, $exceptionStr)
    {
        $trace = pinpoint_get_current_trace();
        pinpoint_log(PINPOINT_DEBUG, "PINPOINT_DEBUG-----");
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

class TestGetSelf1Interceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;
    public function __construct()
    {
        $this->apiId = pinpoint_add_api("test_getSelf1::testFunc", 2); // functionName, lineno
    }

    public function onBefore($callId, $args)
    {
        $trace = pinpoint_get_current_trace();
        if ($trace)
        {
            $self = $this->getSelf();


            $event = $trace->traceBlockBegin($callId);
            $event->markBeforeTime();
            $event->setApiId($this->apiId);
            $event->setServiceType(PINPOINT_PHP_RPC_TYPE);

            if($self)
            {
                echo "$self->name = ".$self->name . "<br/>";

            }
            else
            {
                echo "self is null";
            }

            pinpoint_log(PINPOINT_INFO, "PINPOINT_INFO-----");

            $event->addAnnotation(PINPOINT_ANNOTATION_ARGS,htmlspecialchars(print_r($args,true),ENT_QUOTES));
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
                $event->addAnnotation(PINPOINT_ANNOTATION_RETURN, htmlspecialchars(print_r($retArgs,true),ENT_QUOTES));
                $event->markAfterTime();
                $trace->traceBlockEnd($event);
            }
            pinpoint_log(PINPOINT_ERROR, "PINPOINT_ERROR-----");
        }
    }

    public function onException($callId, $exceptionStr)
    {
        $trace = pinpoint_get_current_trace();
        pinpoint_log(PINPOINT_DEBUG, "PINPOINT_DEBUG-----");
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

class TestGetSelf2Interceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;
    public function __construct()
    {
        $this->apiId = pinpoint_add_api("test_getSelf2::testFunc", 2); // functionName, lineno
    }

    public function onBefore($callId, $args)
    {
        $trace = pinpoint_get_current_trace();
        if ($trace)
        {
            $self = $this->getSelf();


            $event = $trace->traceBlockBegin($callId);
            $event->markBeforeTime();
            $event->setApiId($this->apiId);
            $event->setServiceType(PINPOINT_PHP_RPC_TYPE);

            if($self)
            {
                echo "$self->name = ".$self->name . "<br/>";

            }
            else
            {
                echo "self is null";
            }

            pinpoint_log(PINPOINT_INFO, "PINPOINT_INFO-----");

            $event->addAnnotation(PINPOINT_ANNOTATION_ARGS,htmlspecialchars(print_r($args,true),ENT_QUOTES));
            //
        }
    }

    public function onEnd($callId, $data)
    {
        $self = $this->getSelf();
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

                if($self)
                {
                    echo "\$self->name = ".$self->name . "<br/>";

                }
                else
                {
                    echo "self is null";
                }

                $trace->traceBlockEnd($event);
            }
            pinpoint_log(PINPOINT_ERROR, "PINPOINT_ERROR-----");
        }
    }
    public function onException($callId, $exceptionStr)
    {
        $trace = pinpoint_get_current_trace();
        pinpoint_log(PINPOINT_DEBUG, "PINPOINT_DEBUG-----");
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

class TestGetSelf3Interceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;

    public function __construct()
    {
        $this->apiId = pinpoint_add_api("test_getSelf3::testFunc", 2); // functionName, lineno
    }

    public function onBefore($callId, $args)
    {
        $trace = pinpoint_get_current_trace();
        if ($trace)
        {
            $self = $this->getSelf();


            $event = $trace->traceBlockBegin($callId);
            $event->markBeforeTime();
            $event->setApiId($this->apiId);
            $event->setServiceType(PINPOINT_PHP_RPC_TYPE);

            pinpoint_log(PINPOINT_INFO, "PINPOINT_INFO-----");

            $event->addAnnotation(PINPOINT_ANNOTATION_ARGS,htmlspecialchars(print_r($args,true),ENT_QUOTES));
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
                $event->addAnnotation(PINPOINT_ANNOTATION_RETURN, htmlspecialchars(print_r($retArgs,true),ENT_QUOTES));
                $event->markAfterTime();

                $trace->traceBlockEnd($event);
            }
            pinpoint_log(PINPOINT_ERROR, "PINPOINT_ERROR-----");
        }
    }

    public function onException($callId, $exceptionStr)
    {
        $self = $this->getSelf();
        $trace = pinpoint_get_current_trace();
        pinpoint_log(PINPOINT_DEBUG, "PINPOINT_DEBUG-----");
        if ($trace)
        {
            $event = $trace->getEvent($callId);
            if ($event)
            {
                $event->markAfterTime();
                $event->setExceptionInfo($exceptionStr);
                if($self)
                {
                    echo "\$self->name = ".$self->name . "<br/>";

                }
                else
                {
                    echo "self is null<br/>";
                }
//                $trace->traceBlockEnd($event);
            }
        }
    }
}

class TestGetSelf4Interceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;

    public function __construct()
    {
        $this->apiId = pinpoint_add_api("test_getSelf4::testFunc", 2); // functionName, lineno
    }

    public function onBefore($callId, $args)
    {
        $trace = pinpoint_get_current_trace();
        if ($trace)
        {
            echo "Enter Exception!<br/>";
            $event = $trace->traceBlockBegin($callId);
            $event->markBeforeTime();
            $event->setApiId($this->apiId);
            $event->setServiceType(PINPOINT_PHP_RPC_TYPE);

            pinpoint_log(PINPOINT_INFO, "PINPOINT_INFO-----");

            $event->addAnnotation(PINPOINT_ANNOTATION_ARGS,htmlspecialchars(print_r($args,true),ENT_QUOTES));
            //
        }
    }

    public function onEnd($callId, $data)
    {
        $self = $this->getSelf();
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

                if($self)
                {
                    echo "\$self->name = ".$self->name . "<br/>";

                }
                else
                {
                    echo "self is null<br/>";
                }

                $trace->traceBlockEnd($event);
            }
            pinpoint_log(PINPOINT_ERROR, "PINPOINT_ERROR-----");
        }
    }

    public function onException($callId, $exceptionStr)
    {

        $trace = pinpoint_get_current_trace();
        pinpoint_log(PINPOINT_DEBUG, "PINPOINT_DEBUG-----");
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

class TestGetSelf5Interceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;

    public function __construct()
    {
        $this->apiId = pinpoint_add_api("test_getSelf5::testFunc", 2); // functionName, lineno
    }

    public function onBefore($callId, $args)
    {
        $trace = pinpoint_get_current_trace();
        if ($trace)
        {
            $self = $this->getSelf();


            $event = $trace->traceBlockBegin($callId);
            $event->markBeforeTime();
            $event->setApiId($this->apiId);
            $event->setServiceType(PINPOINT_PHP_RPC_TYPE);

            pinpoint_log(PINPOINT_INFO, "PINPOINT_INFO-----");

            $event->addAnnotation(PINPOINT_ANNOTATION_ARGS,htmlspecialchars(print_r($args,true),ENT_QUOTES));
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
                $event->addAnnotation(PINPOINT_ANNOTATION_RETURN, htmlspecialchars(print_r($retArgs,true),ENT_QUOTES));
                $event->markAfterTime();

                $trace->traceBlockEnd($event);
            }
            pinpoint_log(PINPOINT_ERROR, "PINPOINT_ERROR-----");
        }
    }

    public function onException($callId, $exceptionStr)
    {
        $self = $this->getSelf();
        $trace = pinpoint_get_current_trace();
        pinpoint_log(PINPOINT_DEBUG, "PINPOINT_DEBUG-----");
        if ($trace)
        {
            $event = $trace->getEvent($callId);
            if ($event)
            {
                $event->markAfterTime();
                $event->setExceptionInfo($exceptionStr);
                if($self)
                {
                    echo "\$self->name = ".$self->name . "<br/>";

                }
                else
                {
                    echo "self is null<br/>";
                }
//                $trace->traceBlockEnd($event);
            }
        }
    }
}

//Exit in onBefore.
class TestExit01Interceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;

    public function __construct()
    {
        $this->apiId = pinpoint_add_api("test_exit01::testFunc", 2); // functionName, lineno
    }

    public function onBefore($callId, $args)
    {
        $trace = pinpoint_get_current_trace();
        if ($trace)
        {
            exit();
            $event = $trace->traceBlockBegin($callId);
            $event->markBeforeTime();
            $event->setApiId($this->apiId);
            $event->setServiceType(PINPOINT_PHP_RPC_TYPE);

            pinpoint_log(PINPOINT_INFO, "PINPOINT_INFO-----");

            $event->addAnnotation(PINPOINT_ANNOTATION_ARGS,htmlspecialchars(print_r($args,true),ENT_QUOTES));
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
                $event->addAnnotation(PINPOINT_ANNOTATION_RETURN, htmlspecialchars(print_r($retArgs,true),ENT_QUOTES));
                $event->markAfterTime();

                $trace->traceBlockEnd($event);
            }
            pinpoint_log(PINPOINT_ERROR, "PINPOINT_ERROR-----");
        }
    }

    public function onException($callId, $exceptionStr)
    {
        // exit();
        $trace = pinpoint_get_current_trace();
        pinpoint_log(PINPOINT_DEBUG, "PINPOINT_DEBUG-----");
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

// Exit in onEnd.
class TestExit02Interceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;

    public function __construct()
    {
        $this->apiId = pinpoint_add_api("test_exit02::testFunc", 2); // functionName, lineno
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

            pinpoint_log(PINPOINT_INFO, "PINPOINT_INFO-----");

            $event->addAnnotation(PINPOINT_ANNOTATION_ARGS,htmlspecialchars(print_r($args,true),ENT_QUOTES));
            //
        }
    }

    public function onEnd($callId, $data)
    {
        exit();
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
            pinpoint_log(PINPOINT_ERROR, "PINPOINT_ERROR-----");
        }
    }

    public function onException($callId, $exceptionStr)
    {
        $trace = pinpoint_get_current_trace();
        pinpoint_log(PINPOINT_DEBUG, "PINPOINT_DEBUG-----");
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

//Exit in onException.
class TestExit03Interceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;

    public function __construct()
    {
        $this->apiId = pinpoint_add_api("test_exit03::testFunc", 2); // functionName, lineno
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

            pinpoint_log(PINPOINT_INFO, "PINPOINT_INFO-----");

            $event->addAnnotation(PINPOINT_ANNOTATION_ARGS,htmlspecialchars(print_r($args,true),ENT_QUOTES));
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
                $event->addAnnotation(PINPOINT_ANNOTATION_RETURN, htmlspecialchars(print_r($retArgs,true),ENT_QUOTES));
                $event->markAfterTime();

                $trace->traceBlockEnd($event);
            }
            pinpoint_log(PINPOINT_ERROR, "PINPOINT_ERROR-----");
        }
    }

    public function onException($callId, $exceptionStr)
    {
        exit();
        $trace = pinpoint_get_current_trace();
        pinpoint_log(PINPOINT_DEBUG, "PINPOINT_DEBUG-----");
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

//Exit in function.
class TestExit04Interceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;

    public function __construct()
    {
        $this->apiId = pinpoint_add_api("test_exit04::testFunc", 2); // functionName, lineno
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

            pinpoint_log(PINPOINT_INFO, "PINPOINT_INFO-----");

            $event->addAnnotation(PINPOINT_ANNOTATION_ARGS,htmlspecialchars(print_r($args,true),ENT_QUOTES));
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
                $event->addAnnotation(PINPOINT_ANNOTATION_RETURN, htmlspecialchars(print_r($retArgs,true),ENT_QUOTES));
                $event->markAfterTime();

                $trace->traceBlockEnd($event);
            }
            pinpoint_log(PINPOINT_ERROR, "PINPOINT_ERROR-----");
        }
    }

    public function onException($callId, $exceptionStr)
    {
        $trace = pinpoint_get_current_trace();
        pinpoint_log(PINPOINT_DEBUG, "PINPOINT_DEBUG-----");
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

//Exception throws in onBefore.
class TestException01Interceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;

    public function __construct()
    {
        $this->apiId = pinpoint_add_api("test_exception01::testFunc", 2); // functionName, lineno
    }

    public function onBefore($callId, $args)
    {

        throw new Exception("Error Processing Request", 1);

        $trace = pinpoint_get_current_trace();
        if ($trace)
        {
            $event = $trace->traceBlockBegin($callId);
            $event->markBeforeTime();
            $event->setApiId($this->apiId);
            $event->setServiceType(PINPOINT_PHP_RPC_TYPE);

            pinpoint_log(PINPOINT_INFO, "PINPOINT_INFO-----");

            $event->addAnnotation(PINPOINT_ANNOTATION_ARGS,htmlspecialchars(print_r($args,true),ENT_QUOTES));

        }
    }

    public function onEnd($callId, $data)
    {
        // exit();
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
            pinpoint_log(PINPOINT_ERROR, "PINPOINT_ERROR-----");
        }
    }

    public function onException($callId, $exceptionStr)
    {
        // exit();
        $trace = pinpoint_get_current_trace();
        pinpoint_log(PINPOINT_DEBUG, "PINPOINT_DEBUG-----");
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

//Exception throws in onEnd.
class TestException02Interceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;

    public function __construct()
    {
        $this->apiId = pinpoint_add_api("test_exception02::testFunc", 2); // functionName, lineno
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

            pinpoint_log(PINPOINT_INFO, "PINPOINT_INFO-----");

            $event->addAnnotation(PINPOINT_ANNOTATION_ARGS,htmlspecialchars(print_r($args,true),ENT_QUOTES));

        }
    }

    public function onEnd($callId, $data)
    {
        throw new Exception("Error Processing Request", 1);
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
            pinpoint_log(PINPOINT_ERROR, "PINPOINT_ERROR-----");
        }
    }

    public function onException($callId, $exceptionStr)
    {

        $trace = pinpoint_get_current_trace();
        pinpoint_log(PINPOINT_DEBUG, "PINPOINT_DEBUG-----");
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

//Exception throws in onException.
class TestException03Interceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;

    public function __construct()
    {
        $this->apiId = pinpoint_add_api("test_exception03::testFunc", 2); // functionName, lineno
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

            pinpoint_log(PINPOINT_INFO, "PINPOINT_INFO-----");

            $event->addAnnotation(PINPOINT_ANNOTATION_ARGS,htmlspecialchars(print_r($args,true),ENT_QUOTES));

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
            pinpoint_log(PINPOINT_ERROR, "PINPOINT_ERROR-----");
        }
    }

    public function onException($callId, $exceptionStr)
    {
        throw new Exception("Error Processing Request", 1);
        $trace = pinpoint_get_current_trace();
        pinpoint_log(PINPOINT_DEBUG, "PINPOINT_DEBUG-----");
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

//Invoke a undefined function in onBefore.
class TestError01Interceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;

    public function __construct()
    {
        $this->apiId = pinpoint_add_api("test_error01::testFunc", 2); // functionName, lineno
    }

    public function onBefore($callId, $args)
    {

        xxx();

        $trace = pinpoint_get_current_trace();
        if ($trace)
        {
            $event = $trace->traceBlockBegin($callId);
            $event->markBeforeTime();
            $event->setApiId($this->apiId);
            $event->setServiceType(PINPOINT_PHP_RPC_TYPE);

            pinpoint_log(PINPOINT_INFO, "PINPOINT_INFO-----");

            $event->addAnnotation(PINPOINT_ANNOTATION_ARGS,htmlspecialchars(print_r($args,true),ENT_QUOTES));

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
            pinpoint_log(PINPOINT_ERROR, "PINPOINT_ERROR-----");
        }
    }

    public function onException($callId, $exceptionStr)
    {

        $trace = pinpoint_get_current_trace();
        pinpoint_log(PINPOINT_DEBUG, "PINPOINT_DEBUG-----");
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

//Invoke a undefined function in onEnd.
class TestError02Interceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;

    public function __construct()
    {
        $this->apiId = pinpoint_add_api("test_error02::testFunc", 2); // functionName, lineno
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

            pinpoint_log(PINPOINT_INFO, "PINPOINT_INFO-----");

            $event->addAnnotation(PINPOINT_ANNOTATION_ARGS,htmlspecialchars(print_r($args,true),ENT_QUOTES));

        }
    }

    public function onEnd($callId, $data)
    {
        xxx();

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
            pinpoint_log(PINPOINT_ERROR, "PINPOINT_ERROR-----");
        }
    }

    public function onException($callId, $exceptionStr)
    {

        $trace = pinpoint_get_current_trace();
        pinpoint_log(PINPOINT_DEBUG, "PINPOINT_DEBUG-----");
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

//Invoke a undefined function in onException.
class TestError03Interceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;

    public function __construct()
    {
        $this->apiId = pinpoint_add_api("test_error03::testFunc", 2); // functionName, lineno
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

            pinpoint_log(PINPOINT_INFO, "PINPOINT_INFO-----");

            $event->addAnnotation(PINPOINT_ANNOTATION_ARGS,htmlspecialchars(print_r($args,true),ENT_QUOTES));

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
            pinpoint_log(PINPOINT_ERROR, "PINPOINT_ERROR-----");
        }
    }

    public function onException($callId, $exceptionStr)
    {
        xxx();
        $trace = pinpoint_get_current_trace();
        pinpoint_log(PINPOINT_DEBUG, "PINPOINT_DEBUG-----");
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

#ISSUE115.
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

            pinpoint_log(PINPOINT_INFO, "PINPOINT_INFO-----");

            $event->addAnnotation(PINPOINT_ANNOTATION_ARGS,htmlspecialchars(print_r($args,true),ENT_QUOTES));

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
            pinpoint_log(PINPOINT_ERROR, "PINPOINT_ERROR-----");
        }
    }

    public function onException($callId, $exceptionStr)
    {

        $trace = pinpoint_get_current_trace();
        pinpoint_log(PINPOINT_DEBUG, "PINPOINT_DEBUG-----");
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

#Warning in onBefore.
class TestWarning01Interceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;

    public function __construct()
    {
        $this->apiId = pinpoint_add_api("test_Warning01::testFunc", 2); // functionName, lineno
    }

    public function onBefore($callId,$args)
    {
        $a=1;
        echo $a/0;

        $trace = pinpoint_get_current_trace();
        if ($trace)
        {
            $event = $trace->traceBlockBegin($callId);
            $event->markBeforeTime();
            $event->setApiId($this->apiId);
            $event->setServiceType(PINPOINT_PHP_RPC_TYPE);

            pinpoint_log(PINPOINT_INFO, "PINPOINT_INFO-----");

            $event->addAnnotation(PINPOINT_ANNOTATION_ARGS,htmlspecialchars(print_r($args,true),ENT_QUOTES));

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
            pinpoint_log(PINPOINT_ERROR, "PINPOINT_ERROR-----");
        }
    }

    public function onException($callId, $exceptionStr)
    {

        $trace = pinpoint_get_current_trace();
        pinpoint_log(PINPOINT_DEBUG, "PINPOINT_DEBUG-----");
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

#Warning in onEnd.
class TestWarning02Interceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;

    public function __construct()
    {
        $this->apiId = pinpoint_add_api("test_Warning02::testFunc", 2); // functionName, lineno
    }

    public function onBefore($callId,$args)
    {


        $trace = pinpoint_get_current_trace();
        if ($trace)
        {
            $event = $trace->traceBlockBegin($callId);
            $event->markBeforeTime();
            $event->setApiId($this->apiId);
            $event->setServiceType(PINPOINT_PHP_RPC_TYPE);

            pinpoint_log(PINPOINT_INFO, "PINPOINT_INFO-----");

            $event->addAnnotation(PINPOINT_ANNOTATION_ARGS,htmlspecialchars(print_r($args,true),ENT_QUOTES));

        }
    }

    public function onEnd($callId, $data)
    {

        $trace = pinpoint_get_current_trace();

        $a=1;
        echo $a/0;
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
            pinpoint_log(PINPOINT_ERROR, "PINPOINT_ERROR-----");
        }
    }

    public function onException($callId, $exceptionStr)
    {

        $trace = pinpoint_get_current_trace();
        pinpoint_log(PINPOINT_DEBUG, "PINPOINT_DEBUG-----");
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

#Warning in onException.
class TestWarning03Interceptor extends \Pinpoint\Interceptor
{
    var $apiId = -1;

    public function __construct()
    {
        $this->apiId = pinpoint_add_api("test_Warning03::testFunc", 2); // functionName, lineno
    }

    public function onBefore($callId,$args)
    {


        $trace = pinpoint_get_current_trace();
        if ($trace)
        {
            $event = $trace->traceBlockBegin($callId);
            $event->markBeforeTime();
            $event->setApiId($this->apiId);
            $event->setServiceType(PINPOINT_PHP_RPC_TYPE);

            pinpoint_log(PINPOINT_INFO, "PINPOINT_INFO-----");

            $event->addAnnotation(PINPOINT_ANNOTATION_ARGS,htmlspecialchars(print_r($args,true),ENT_QUOTES));

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
            pinpoint_log(PINPOINT_ERROR, "PINPOINT_ERROR-----");
        }
    }

    public function onException($callId, $exceptionStr)
    {
        $a=1;
        echo $a/0;
        $trace = pinpoint_get_current_trace();
        pinpoint_log(PINPOINT_DEBUG, "PINPOINT_DEBUG-----");
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
        // you have to call the construct of parent class here
        parent::__construct();

        //Test onEnd01
        $i = new TestOnEnd01Interceptor();
        $this->addInterceptor($i, "test_onEnd1::testFunc", "test_01_plugin.php");

        //Test onEnd02
        $i = new TestOnEnd02Interceptor();
        $this->addInterceptor($i, "test_onEnd2::testFunc", "test_01_plugin.php");

        //Test onEnd03
        $i = new TestOnEnd03Interceptor();
        $this->addInterceptor($i, "test_onEnd3::testFunc", "test_01_plugin.php");

        //Test onEnd04
        $i = new TestOnEnd04Interceptor();
        $this->addInterceptor($i, "test_onEnd4::testFunc", "test_01_plugin.php");

        //Pinpoing-c-agent-0.0.3-TC-011
        $i = new TestGetSelf1Interceptor();
        $this->addInterceptor($i, "test_getSelf1::testFunc", "test_01_plugin.php");

        ////Pinpoing-c-agent-0.0.3-TC-012
        $i = new TestGetSelf2Interceptor();
        $this->addInterceptor($i, "test_getSelf2::testFunc", "test_01_plugin.php");

        //Pinpoing-c-agent-0.0.3-TC-014
        $i = new TestGetSelf3Interceptor();
        $this->addInterceptor($i, "test_getSelf3::testFunc", "test_01_plugin.php");

        //Pinpoing-c-agent-0.0.3-TC-013
        $i = new TestGetSelf4Interceptor();
        $this->addInterceptor($i, "test_getSelf4::testFunc", "test_01_plugin.php");

        //Pinpoing-c-agent-0.0.3-TC-015
        $i = new TestGetSelf5Interceptor();
        $this->addInterceptor($i, "test_getSelf5::testFunc", "test_01_plugin.php");

        //Pinpoing-c-agent-0.0.3-TC-016
        $i = new TestExit01Interceptor();
        $this->addInterceptor($i, "test_exit01::testFunc", "test_01_plugin.php");

        //Pinpoing-c-agent-0.0.3-TC-017
        $i = new TestExit02Interceptor();
        $this->addInterceptor($i, "test_exit02::testFunc", "test_01_plugin.php");

        //Pinpoing-c-agent-0.0.3-TC-018
        $i = new TestExit03Interceptor();
        $this->addInterceptor($i, "test_exit03::testFunc", "test_01_plugin.php");

        //Pinpoing-c-agent-0.0.3-TC-019
        $i = new TestExit04Interceptor();
        $this->addInterceptor($i, "test_exit04::testFunc", "test_01_plugin.php");

        //Pinpoing-c-agent-0.0.3-TC-020
        $i = new TestException01Interceptor();
        $this->addInterceptor($i, "test_exception01::testFunc", "test_01_plugin.php");

        //Pinpoing-c-agent-0.0.3-TC-021
        $i = new TestException02Interceptor();
        $this->addInterceptor($i, "test_exception02::testFunc", "test_01_plugin.php");

        //Pinpoing-c-agent-0.0.3-TC-022
        $i = new TestException03Interceptor();
        $this->addInterceptor($i, "test_exception03::testFunc", "test_01_plugin.php");

        //Pinpoing-c-agent-0.0.3-TC-023
        $i = new TestError01Interceptor();
        $this->addInterceptor($i, "test_error01::testFunc", "test_01_plugin.php");

        //Pinpoing-c-agent-0.0.3-TC-024
        $i = new TestError02Interceptor();
        $this->addInterceptor($i, "test_error02::testFunc", "test_01_plugin.php");

        //Pinpoing-c-agent-0.0.3-TC-025
        $i = new TestError03Interceptor();
        $this->addInterceptor($i, "test_error03::testFunc", "test_01_plugin.php");

        //issue115
        $i = new TestISSUE115Interceptor();
        $this->addInterceptor($i, "test_issue115::testFunc", "test_01_plugin.php");

        //Pinpoing-c-agent-0.0.3-TC-026
        $i = new TestWarning01Interceptor();
        $this->addInterceptor($i, "test_Warning01::testFunc", "test_01_plugin.php");

        //Pinpoing-c-agent-0.0.3-TC-027
        $i = new TestWarning02Interceptor();
        $this->addInterceptor($i, "test_Warning02::testFunc", "test_01_plugin.php");

        //Pinpoing-c-agent-0.0.3-TC-028
        $i = new TestWarning03Interceptor();
        $this->addInterceptor($i, "test_Warning03::testFunc", "test_01_plugin.php");

    }
}
