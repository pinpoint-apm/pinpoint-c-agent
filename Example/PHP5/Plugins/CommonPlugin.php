<?php
#-------------------------------------------------------------------------------
# Copyright 2019 NAVER Corp
# 
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License.  You may obtain a copy
# of the License at
# 
#   http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations under
# the License.
#-------------------------------------------------------------------------------

namespace Plugins;
use Plugins\Candy;

///@hook:app\Foo::foo_p3_rbool app\Foo::foo_p3_rfloat app\Foo::foo_p3 app\Foo::foo_p1
/// @hook:test wrong format
///@hook:app\AppDate::\date app1\Foo::test_func_2 app\AopFunction::test_func1 app\TestError::Strict
///@hook:app\AppDate::outputDate
///@hook:app\AppDate::\date
///@hook:app\AopFunction::test_func1
///@hook:app\AopFunction::test_func2  app1\Foo::test_func_2
///@hook:app\Person::eat app\Teacher::eat app\Student::eat app\Doctor::other
///@hook:app\TestGenerator::generator
///@hook:app\AbstractStudent::eat app\AbstractStudent::drink app\AbstractStudent::breath
///@hook:app\AbstractStudent::eat app\AbstractStudent::drink app\AbstractPerson::breath
///@hook:app\InterfaceStudent::setVariable app\InterfaceStudent::getHtml app\InterfaceStudent::other
///@hook:app\TestClone::setColor app\TestClone::getColor app\TestClone::__clone
///@hook:app\TestRecursion::test_cumsum1 app\TestRecursion::test_cumsum2
///@hook:app\TestAnonymous::createbell app\AopFunction::test_func1
///@hook:app\TestStatic::static_func
///@hook:app\TestFinalClass::test app\TestFinalFunc::test
///@hook:app\FuncInTrait::traitfunc app\TestTrait::test
///@hook:app\CallBackFunc::fnCallback1 app\CallBackFunc::fnCallback2
///@hook:app\LevelClass::public_function app\LevelClass::protected_function app\LevelClass::private_function
///@hook:app\TestError::throwException app\TestError::caughtException app\TestError::testRetConst app\TestError::testRetParm app\TestError::testRetConstAndExp app\TestError::testParseError app\TestError::testTypeError app\TestError::testArithmeticError app\TestError::testDivisionByZeroError app\TestError::testAssertionError
///@hook:app\OverRideLevel::public_function app\OverRideLevel::protected_function app\OverRideLevel::private_function
///@hook:app\TestArgs::test_args
///@hook:app\TestReturn::test_return
class CommonPlugin extends Candy
{
    ///@hook:app\DBcontrol::connectDb
    public function onBefore(){
        pinpoint_add_clue("stp",PHP_METHOD);
        pinpoint_add_clues(PHP_ARGS,print_r($this->args,true));
    }

    ///@hook:app\DBcontrol::getData1 app\DBcontrol::\array_push
    public function onEnd(&$ret){
        pinpoint_add_clues(PHP_RETURN,print_r($ret,true));
    }

    ///@hook:app\DBcontrol::getData2
    public function onException($e){
        pinpoint_add_clue("EXP",$e->getMessage());
    }
}
