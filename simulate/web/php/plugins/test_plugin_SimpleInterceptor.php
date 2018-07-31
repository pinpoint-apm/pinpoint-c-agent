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

class TestPlugin002 extends \Pinpoint\Plugin
{
    public function __construct()
    {
        // you have to call the construct of parent class here
        parent::__construct();

//        $this->addSimpleInterceptor("date", -1);
//        $this->addSimpleInterceptor("test_func1", -1);
//        $this->addSimpleInterceptor("testNameSpace\\test_func2", 5);

        // call_user_func this function can't trace in php7 , so remove it early
//          $this->addSimpleInterceptor("call_user_func", -1);

////        Test exception
//        $this->addSimpleInterceptor("test_exception1", -1);
//        $this->addSimpleInterceptor("test_exception2", -1);

        //Test error
        $this->addSimpleInterceptor("error", -1);

        //Test call back function
        $this->addSimpleInterceptor("MyClass::fnCallBack", -1);

        //Test inherit
        $this->addSimpleInterceptor("Person::eat", -1);

        //Test generator
        $this->addSimpleInterceptor("test_generator::generator", -1);

        //Test Abstract Class
        $this->addSimpleInterceptor("Student::eat", -1);
        $this->addSimpleInterceptor("Student::drink", -1);
        $this->addSimpleInterceptor("Student::breath", -1);
        $this->addSimpleInterceptor("Person::breath", -1);

        //Test namespace
        $this->addSimpleInterceptor("testNameSpace\\sam\\Circle::area", -1);
        $this->addSimpleInterceptor("testNameSpace\\sam\\Circle::perimeter", -1);

        //Test Clone
        $this->addSimpleInterceptor("Television::getColor", -1);

        //Test Connect to Redis
        // $this->addSimpleInterceptor("Redis::keys", -1);

        //Test Connect to MySQL
        $this->addSimpleInterceptor("mysqli::query", -1);

        //PHP 7 New Features
        //return
        $this->addSimpleInterceptor("arraySum", -1);
        //Test anonymous class
        $this->addSimpleInterceptor("Application::setLogger", -1);

        $this->addSimpleInterceptor("test_cumsum2", -1);
        $this->addSimpleInterceptor("test_cumsum4", -1);
        $this->addSimpleInterceptor("test_cumsum_e2", -1);
        $this->addSimpleInterceptor("test_exception3", -1);

        $this->addSimpleInterceptor("Redis::connect", -1);
        $this->addSimpleInterceptor("test_simple_interceptor", -1);
        $this->addSimpleInterceptor("Redis::set", -1);
        $this->addSimpleInterceptor("Redis::get", -1);
        $this->addSimpleInterceptor("Redis::delete", -1);
        $this->addSimpleInterceptor("Redis::keys", -1);

    }
}