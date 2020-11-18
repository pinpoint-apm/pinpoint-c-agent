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
 * SpanConnectionPool.cpp
 *
 *  Created on: Aug 28, 2020
 *      Author: eeliu
 */

#include "SpanConnectionPool.h"
#include "Cache/SafeSharedState.h"
#include <sstream> 

namespace ConnectionPool {
using Cache::SafeSharedState;

SpanConnectionPool::SpanConnectionPool(const char* co_host,uint32_t w_timeout_ms):
        co_host(co_host),
        timeout_ms(w_timeout_ms),
        con_counter(0)
{
    this->_cPool.push(this->createTrans());
}


TransConnection SpanConnectionPool::createTrans()
{
    TransConnection _connect( new TransLayer(this->co_host,this->timeout_ms));
    using namespace std::placeholders;
    // _connect->registerPeerMsgCallback(
    //     std::bind(&SpanConnectionPool::_handleMsgFromCollector,this,_1,_2,_3),
    //     std::bind(&SpanConnectionPool::_handleTransLayerState,this,_1));

    _connect->registerPeerMsgCallback(
        std::bind(&SpanConnectionPool::_handleMsgFromCollector,this,_1,_2,_3),
        nullptr);
    this->con_counter++;

    return _connect;
}

//@obsoleted
// void SpanConnectionPool::_handleTransLayerState(int state)
// {
//     if(state == E_OFFLINE)
//     {
//         if(global_agent_info.inter_flag & E_UTEST){
//             SafeSharedState::instance().markONLine();
//         }
//         else{
//             SafeSharedState::instance().markOFFLine();
//         }
//     }
// }



void SpanConnectionPool::_handleMsgFromCollector(int type,const char* buf,size_t len)
{
    switch (type)
    {

    case RESPONSE_AGENT_INFO:
        this->_handle_agent_info(type,buf,len);
        break;
    
    default:
        break;
    }

}

void SpanConnectionPool::_handle_agent_info(int type,const char* buf,size_t len)
{
    Json::Value  root;

    Json::CharReaderBuilder builder;
    builder["collectComments"] = false;

    std::istringstream inss(std::string(buf,len));
    JSONCPP_STRING errs;
    bool ok = parseFromStream(builder, inss, &root, &errs);

    if(ok)
    {
        // pp_trace("collector-agent say:%s",root.toStyledString().c_str());

        if(root.isMember("time")){
            SafeSharedState::instance().updateStartTime(atoll(root["time"].asCString()));
        }
        
    }else{
        pp_trace("Recieve invalid msg: %.*s from Collector-agent, reason: %s",len,buf,errs.c_str());
    }

}


} /* namespace ConnectionPool */
