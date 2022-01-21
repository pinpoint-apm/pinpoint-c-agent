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

namespace Plugins\AutoGen\app;
use Pinpoint\Plugins\Common\PinTrace;

///@hook:app\common\AppDate::outputDate
///@hook:app\common\AopFunction::test_func1
///@hook:app\common\AopFunction::test_func2  app\util\Foo::test_func_2
///@hook:app\common\Person::eat app\common\Teacher::eat app\common\Student::eat app\common\Doctor::other app\common\Doctor::eat
///@hook:app\common\AbstractStudent::eat app\common\AbstractStudent::drink app\common\AbstractStudent::breath
///@hook:app\common\AbstractStudent::eat app\common\AbstractStudent::drink app\common\AbstractPerson::breath
///@hook:app\common\InterfaceStudent::setVariable app\common\InterfaceStudent::getHtml app\common\InterfaceStudent::other
///@hook:app\common\TestClone::setColor app\common\TestClone::getColor app\common\TestClone::__clone
///@hook:app\common\TestRecursion::test_cumsum1 app\common\TestRecursion::test_cumsum2
///@hook:app\common\TestAnonymous::createbell
///@hook:app\common\TestStatic::static_func
///@hook:app\common\TestFinalClass::test app\common\TestFinalFunc::test
///@hook:app\common\FuncInTrait::traitfunc app\common\TestTrait::test
///@hook:app\common\CallBackFunc::fnCallback1 app\common\CallBackFunc::fnCallback2
///@hook:app\common\LevelClass::public_function app\common\LevelClass::protected_function app\common\LevelClass::private_function
///@hook:app\common\OverRideLevel::public_function app\common\OverRideLevel::protected_function app\common\OverRideLevel::private_function
///@hook:app\common\TestArgs::test_args
///@hook:app\common\TestReturn::test_return
///@hook:app\common\TestError::throwException app\common\TestError::test_cumsum_e1 app\common\TestError::caughtException app\common\TestError::error app\common\TestError::testParseError app\common\TestError::testTypeError app\common\TestError::testArithmeticError app\common\TestError::testDivisionByZeroError app\common\TestError::testAssertionError
class CommonPlugin extends PinTrace
{
    ///@hook:app\common\DBcontrol::connectDb
    public function onBefore(){
        pinpoint_add_clue("stp",PP_PHP_METHOD);
        pinpoint_add_clues(PP_PHP_ARGS,print_r($this->args,true));
    }

    ///@hook:app\common\DBcontrol::getData1 app\common\DBcontrol::\array_push
    public function onEnd(&$ret){
        pinpoint_add_clues(PP_PHP_RETURN,print_r($ret,true));
    }

    ///@hook:app\common\DBcontrol::getData2
    public function onException($e){
        pinpoint_add_clue("EXP",$e->getMessage());
    }
}
