////////////////////////////////////////////////////////////////////////////////
// Copyright 2020 NAVER Corp
// 
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License.  You may obtain a copy
// of the License at
// 
//   http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
// License for the specific language governing permissions and limitations under
// the License.
////////////////////////////////////////////////////////////////////////////////
/*
 * ContextType.h
 *
 *  Created on: Aug 27, 2020
 *      Author: eeliu
 */

#ifndef COMMON_SRC_CONTEXTTYPE_H_
#define COMMON_SRC_CONTEXTTYPE_H_

#include <stdint.h>
#include <string>
#include <stdexcept>

namespace Context {



typedef const char* ctype;

class ContextType
{
public:
    ContextType(){}
    virtual ~ContextType(){}

    virtual ctype typeIs() { throw std::logic_error("not implementation");}
    virtual std::string& asStringValue()  { throw std::logic_error("not implementation");}
    virtual long asLongValue() { throw std::logic_error("not implementation"); }
};


class StringContextType: public ContextType{
public:
    StringContextType(std::string&& value):
    _value(std::move(value))
    {
    }

    virtual ctype typeIs() override
    {
        return "String";
    }

    virtual std::string& asStringValue() override
    {
        return this->_value;
    }

    virtual ~StringContextType()
    {

    }

private:
    std::string _value;
};


class LongContextType: public ContextType
{
public:
    LongContextType(long l):
    l(l)
    {}

    virtual ctype typeIs() override
    {
        return "Long";
    }

    virtual long asLongValue() override
    {
        return this->l;
    }

private:
    long l;
};

} /* namespace Context */

#endif /* COMMON_SRC_CONTEXTTYPE_H_ */
