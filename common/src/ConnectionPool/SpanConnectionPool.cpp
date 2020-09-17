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

namespace ConnectionPool {

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
    _connect->registerPeerMsgCallback(
        std::bind(&SpanConnectionPool::_handleMsgFromCollector,this,_1,_2,_3),
        std::bind(&SpanConnectionPool::_handleTransLayerState,this,_1));
    return _connect;
}


void SpanConnectionPool::_handleTransLayerState(int state)
{
    
    // if(state == E_OFFLINE)
    // {
    //     if(global_agent_info.inter_flag & E_UTEST){
    //         this->limit = E_TRACE_PASS;
    //     }
    //     else{
    //         this->limit = E_OFFLINE;
    //     }
    //     this->_state->state |= E_OFFLINE;
    // }
}



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
    Json::Reader reader;
    int ret = reader.parse(buf,buf+len,root);
    if(!ret)
    {
        return ;
    }

    // if(root.isMember("time")){
    //     this->_state->starttime= atoll(root["time"].asCString());
    // }

    // if(root.isMember("id")){
    //     this->app_id      =  root["id"].asString();
    // }

    // if(root.isMember("name")){
    //     this->app_name    = root["name"].asString();
    // }

    // this->limit= E_TRACE_PASS;
    // this->_state->state |= E_TRACE_PASS;
    // pp_trace("starttime:%ld appid:%s appname:%s",this->_state->starttime,this->app_id.c_str(),this->app_name.c_str());
    

}


} /* namespace ConnectionPool */
