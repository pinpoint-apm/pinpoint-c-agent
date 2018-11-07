/*******************************************************************************
 * Copyright 2018 NAVER Corp.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License.  You may obtain a copy
 * of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations under
 * the License.
 ******************************************************************************/
#ifndef SIMULATE_CHECKER_HANDLER_H
#define SIMULATE_CHECKER_HANDLER_H

#include <stdint.h>
#include <vector>
#include <boost/shared_ptr.hpp>

class Checker
{
public:
    virtual int32_t check() = 0;
    virtual ~Checker() {}
};

typedef boost::shared_ptr<Checker> CheckerPtr;
typedef std::vector<CheckerPtr> CheckerPtrVec;

extern int32_t get_checkers(CheckerPtrVec& checkers);

#endif
