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
 * SpanConnectionPool.h
 *
 *  Created on: Aug 28, 2020
 *      Author: eeliu
 */

#ifndef COMMON_SRC_CONNECTPOOL_SPANCONNECTIONPOOL_H_
#define COMMON_SRC_CONNECTPOOL_SPANCONNECTIONPOOL_H_
#include <iostream>
#include <stack>
#include <memory>
#include <mutex>
#include "TransLayer.h"
#include "Util/Trace.h"

namespace ConnectionPool {

typedef std::unique_ptr<TransLayer> TransConnection;

class SpanConnectionPool
{
public:
    SpanConnectionPool(const char* co_host,uint32_t w_timeout_ms);
    virtual ~SpanConnectionPool(){}

    TransConnection getConnection()
    {
        std::lock_guard<std::mutex> _safe(this->_lock);
        ADDTRACE();
        if(this->_cPool.empty())
        {
            return TransConnection(new TransLayer(this->co_host,this->timeout_ms));
        }else{
            TransConnection _con = std::move(_cPool.top());
            return _con;
        }
    }

    void giveBackConnection(TransConnection& con)
    {
        std::lock_guard<std::mutex> _safe(this->_lock);
        ADDTRACE();
        this->_cPool.push(std::move(con));
    }

    SpanConnectionPool(const SpanConnectionPool&) = delete;
    SpanConnectionPool& operator=(const SpanConnectionPool&) = delete;
    SpanConnectionPool& operator=(SpanConnectionPool&&) = delete;
private:
    void _handleMsgFromCollector(int type,const char* buf,size_t len);
    
    void _handle_agent_info(int type,const char* buf,size_t len);

    TransConnection createTrans();
    void _handleTransLayerState(int state);
private:
    const std::string co_host;
    uint32_t timeout_ms;
    uint32_t con_counter;
    std::stack<TransConnection> _cPool;
    std::mutex _lock;
};

} /* namespace ConnectionPool */

#endif /* COMMON_SRC_CONNECTPOOL_SPANCONNECTIONPOOL_H_ */
