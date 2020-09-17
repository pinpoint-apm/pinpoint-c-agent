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
        this->_global_state = NULL;
    }

    bool isOFFLine()
    {
        return (this->_global_state->state & E_OFFLINE);
    }


    void markOFFline()
    {
        this->_global_state->state |= E_OFFLINE;
    }

    void markONLine()
    {
        this->_global_state->state &= ~E_OFFLINE;
    }

    bool checkTraceLimit(int64_t timestamp)
    {
        time_t ts = (timestamp != -1) ?(timestamp) :(std::time(NULL));
        
        if( this->_global_state->timestamp != ts )
        {
            this->_global_state->timestamp = ts;
            this->_global_state->tick = 0 ;
            __sync_synchronize();
        }
        else if(this->_global_state->tick >= this->_global_state->maxtracelimit)
        {
            goto BLOCK;
        }else
        {
            __sync_add_and_fetch(&this->_global_state->tick,1);
        }
        return false;
BLOCK:
        pp_trace("This span dropped. max_trace_limit:%d current_tick:%d",this->_global_state->maxtracelimit,this->_global_state->tick);
        return true;
    }

    void updateStartTime(std::time_t startTime)
    {
        this->_global_state->starttime = startTime;
    }

    std::time_t getStartTime()
    {
       return this->_global_state->starttime;
    }


    uint64_t generateUniqueId()
    {
        return __sync_add_and_fetch(&this->_global_state->uid,1);
    }

    void resetUniqueId()
    {
        this->_global_state->uid = 0;
        __sync_synchronize();
    }

private:

    SafeSharedState()
    {
        this->_global_state = (SharedState*)fetch_shared_obj_addr();
    }

    ~SafeSharedState(){
        
    }
    
    SharedState * _global_state;
};

} /* namespace Cache */

#endif /* COMMON_SRC_CACHE_SAFESHAREDSTATE_H_ */
