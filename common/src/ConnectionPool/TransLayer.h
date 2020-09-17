/*******************************************************************************
 * Copyright 2019 NAVER Corp
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
/*
 * PPTransLayer.h
 *
 *  Created on: Jan 6, 2020
 *      Author: eeliu
 */

#ifndef INCLUDE_PPTRANSLAYER_H_
#define INCLUDE_PPTRANSLAYER_H_
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <functional>
#include <iostream>
#include <arpa/inet.h>
#include <iostream>
#include <functional>

#include "common.h"
#include "Cache/Chunk.h"


namespace ConnectionPool {

using Cache::Chunks;

class TransLayer{

enum E_STATE{S_WRITING=0x1,S_READING=0x2,S_ERROR=0x4};



public:
    explicit TransLayer(const std::string& co_host,uint32_t w_timeout_ms):
    co_host(co_host),
    chunks(1024*1024,1024*40),
    w_timeout_ms(w_timeout_ms),
    _state(0),
    c_fd(-1)
    {
    }

    void registerPeerMsgCallback(std::function<void(int type,const char* buf,size_t len)> _peerMsgCallback,
            std::function<void(int state)> chann_error_cb  )
    {
        if(_peerMsgCallback){
            this->peerMsgCallback = _peerMsgCallback;
        }

        if(chann_error_cb)
        {
            this->chann_error_cb =chann_error_cb;
        }

    }

    size_t trans_layer_pool(uint32_t timeout = 0);
    
    void sendMsgToAgent(const std::string &data)
    {
        uint32_t len = data.size();
        if ( this->chunks.copyDataIntoChunks(data.data(),len) != 0)// _state must be writing
        {
            pp_trace("Send buffer is full. size:[%d]",len);
            return ;
        }
        this->_state |=  S_WRITING;
    }

    /**
     * retry in three times
     * @param timeout
     */
    void forceFlushMsg(uint32_t timeout)
    {
#define MAX_RETRY_TIEMS 3
        int retry =0;
        timeout = (timeout >3) ?(timeout):(3);
        while( (this->_state & S_WRITING) && retry < MAX_RETRY_TIEMS )
        {
            this->trans_layer_pool(timeout/3);
            retry ++;
        }
#undef  MAX_RETRY_TIEMS
    }

    ~TransLayer()
    {
        if(this->c_fd != -1)
        {
            close(this->c_fd);
        }
    }

#ifdef UTEST

#else
private:
#endif

    static int connect_stream_remote(const char* remote);

    static int connect_unix_remote(const char* remote);
   
    int connect_remote(const char* statement)
    {
        int fd = -1;
        const char* substring = NULL;
        if(statement == NULL || statement[0] == '\0')
        {
            goto ERROR;
        }

        /// unix
        substring = strcasestr(statement,UNIX_SOCKET);
        if( substring == statement )
        {
            // sizeof = len +1, so substring -> /tmp/collector.sock
            substring = substring + strlen(UNIX_SOCKET);
            fd = connect_unix_remote(substring);
            c_fd = fd;
            goto DONE;
        }

        ///  tcp tcp:localhost:port
        substring = strcasestr(statement,TCP_SOCKET);
        if( substring == statement )
        {
            // sizeof = len +1, so substring -> /tmp/collector.sock
            substring = substring + strlen(TCP_SOCKET);
            fd = TransLayer::connect_stream_remote(substring);
            c_fd = fd;
            goto DONE;
        }

ERROR:
        pp_trace("remote is not valid:%s",statement);
        return -1;
DONE:
        this->_state |= (S_ERROR|S_READING);
        return fd;
    }

    int _send_msg_to_collector()
    {
        return chunks.drainOutWithPipe(std::bind(&TransLayer::_do_write_data,this,std::placeholders::_1,std::placeholders::_2));
    }

    void _reset_remote( )
    {
        if(c_fd > 0)
        {
            pp_trace("reset peer:%d",c_fd);
            close(c_fd);
            c_fd = -1;
            this->_state = 0;
        }

        if(chann_error_cb)
        {
            chann_error_cb(E_OFFLINE);
        }

        chunks.resetChunks();
    }

    int _do_write_data(const char *data,uint32_t length)
    {
        const char* buf = data;
        uint32_t buf_ofs = 0;
        while(buf_ofs < length){
    #ifdef __APPLE__
            ssize_t ret = send(c_fd,buf + buf_ofs,length -buf_ofs ,0);
    #else
            ssize_t ret = send(c_fd,buf + buf_ofs,length -buf_ofs ,MSG_NOSIGNAL);
    #endif

            if(ret > 0){
                buf_ofs += (uint32_t) ret;
            }else if(ret == -1){
                if(errno == EAGAIN || errno == EWOULDBLOCK || errno== EINTR){
                    return buf_ofs;
                }
                pp_trace("%d send data error:(%s) fd:(%d)",__LINE__,strerror(errno),c_fd);
                return -1;
            }
            else{
                pp_trace("%d send data return 0 error:(%s) fd:(%d)",__LINE__,strerror(errno),c_fd);
                return -1;
            }
        }
        this->_state &=  (~S_WRITING);
        return length;
    }

    int _recv_msg_from_collector()
    {
        int next_size = 0;
        while(next_size < IN_MSG_BUF_SIZE){
            int ret = recv(c_fd,in_buf + next_size,IN_MSG_BUF_SIZE -next_size ,0);
            if (ret > 0){
                int total = ret + next_size;
                int msg_offset = handle_msg_from_collector(in_buf,total);
                if(msg_offset < total){
                    next_size = total - msg_offset;
                    memcpy(in_buf,in_buf + msg_offset ,next_size);
                }else{
                    next_size = 0 ;
                }
            }else if(ret == 0){
                // peer close
                return -1;
            }else{
                if(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR){
                    return 0;
                }
                pp_trace("recv with error:%s",strerror(errno));
                return -1;
            }
        }
        return 0;
    }

    int handle_msg_from_collector(const char* buf,size_t len)
    {
        size_t offset = 0;
        while( offset + 8 <= len ){

            Header * header= (Header*)buf;

            uint32_t body_len = ntohl(header->length);

            if( 8+ body_len > len ){
                return offset ;
            }

            uint32_t type = ntohl(header->type);
            if(peerMsgCallback)
            {
                peerMsgCallback(type, buf+8,len - 8);
            }

            // switch(type){
            // case RESPONSE_AGENT_INFO:
            // // TODO add agent_info update
            //     // handle_agent_info(RESPONSE_AGENT_INFO, buf+8,len - 8);
            //     break;
            // default:
            //     pp_trace("unsupport type:%d",type);
            // }

            offset += (8 +body_len );

        }
        return offset;
    }

private:
    const std::string& co_host;
    Chunks        chunks;
    uint32_t          w_timeout_ms;
    int32_t       _state;
    char          in_buf[IN_MSG_BUF_SIZE]= {0};
    std::function<void(int)> chann_error_cb;
    std::function<void(int type,const char* buf,size_t len)> peerMsgCallback;
    const static char* UNIX_SOCKET;
    const static char* TCP_SOCKET ;
public:
    int           c_fd;
};
}

#endif /* INCLUDE_PPTRANSLAYER_H_ */
