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
 * SafeSharedState.h
 *
 *  Created on: Sep 16, 2020
 *      Author: eeliu
 */

#ifndef COMMON_SRC_CACHE_SAFESHAREDSTATE_H_
#define COMMON_SRC_CACHE_SAFESHAREDSTATE_H_
#include "common.h"
#include "SharedObj.h"
#include<ctime>

namespace Cache {

class SafeSharedState
{

typedef struct _SharedState{
        int64_t uid;
        std::time_t timestamp;
        int64_t tick;
        int64_t maxtracelimit;
        uint64_t starttime;
        int64_t state;
    }SharedState;


public:
    static SafeSharedState& instance()
    {
        static SafeSharedState _shared;
        return _shared;
    }

    void free()
    {
        detach_shared_obj();
        this->_global_state = nullptr;
    }

    bool isReady()
    {
        if( this->_global_state != nullptr &&
          (this->_global_state->state & E_READY)){
            return true ;
        }
        return false;
    }


    bool checkTraceLimit(int64_t timestamp);

    void updateStartTime(std::time_t startTime)
    {
        pp_trace("collector-agent starttime:%ld",startTime);
        this->_global_state->starttime = startTime;
        this->_global_state->state |=  E_READY;
    }

    std::time_t getStartTime()
    {
       return this->_global_state->starttime;
    }


    uint64_t generateUniqueId()
    {
        return __sync_fetch_and_add(&this->_global_state->uid,1);
    }

    void resetUniqueId()
    {
        this->_global_state->uid = 0;
        __sync_synchronize();
    }

private:

    SafeSharedState();

    ~SafeSharedState(){
        this->_global_state = nullptr;
        detach_shared_obj();
    }
    
    SharedState * _global_state;
};

} /* namespace Cache */

#endif /* COMMON_SRC_CACHE_SAFESHAREDSTATE_H_ */
