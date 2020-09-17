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
    
    virtual ctype typeIs(){
        throw std::logic_error(" toString not yet implemented.");
    }
    virtual std::string toString(){
        throw std::logic_error(" toString not yet implemented.");
    }
};


class StringContextType: public ContextType{
public:
    virtual ctype typeIs() {
        return "string";
    }

    virtual std::string toString() const
    {
        return this->_value;
    }

private:
    std::string _value;
};

} /* namespace Context */

#endif /* COMMON_SRC_CONTEXTTYPE_H_ */
