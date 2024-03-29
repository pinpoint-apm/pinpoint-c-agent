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
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include "common.h"
#include "Cache/Chunk.h"
#include "sockets.h"

namespace ConnectionPool {
using Cache::Chunks;

// note update chunk watermark size
// for keep large span.
// BIG span is not friendly to pinpoint-web, show everything equals nothing
#define IN_MSG_BUF_SIZE 4096
const uint32_t _chunk_max_size = 10 * 1024 * 1024; // 10M
const uint32_t _chunk_hold_size = 40 * 1024;       // 40k

using MsgHandleFunc = std::function<void(int type, const char* buf, size_t len)>;
using RouterMsgMap = std::map<int, MsgHandleFunc>;
using RouteMapValueType = RouterMsgMap::value_type;

class TransLayer {
  enum E_STATE { S_WRITING = 0x1, S_READING = 0x2, S_ERROR = 0x4 };

public:
  explicit TransLayer(const std::string& co_host)
      : co_host(co_host), chunks(_chunk_max_size, _chunk_hold_size), _state(0), lastConnectTime(0),
        c_fd(-1) {}

  void RegPeerMsgCallback(int type, MsgHandleFunc call_back) { msgRouteMap_[type] = call_back; }
  void RegStatusChangedCallBack(std::function<void(int)> call_back) {
    statusChangedCallback_ = call_back;
  }

  size_t PoolEventOnce(uint32_t);

  void Reset() { ResetMsgHandler(); }

  bool copy_into_send_buffer(const std::string& data);
  /**
   * retry in three times
   * @param timeout
   */
  void SyncSendAll(uint32_t maxTimeout) {
#define MAX_RETRY_ITEMS 20
    int retry = 0;
    while ((this->_state & S_WRITING) && retry < MAX_RETRY_ITEMS) {
      this->PoolEventOnce(maxTimeout);
      retry++;
    }
#undef MAX_RETRY_ITEMS
  }

  ~TransLayer() {
    if (this->c_fd != -1) {
      close_socket(this->c_fd);
    }
  }

#ifndef UTEST
private:
#endif

  static SOCKET connect_stream_remote(const char* remote);
#ifdef __linux__
  static int connect_unix_remote(const char* remote);
#endif
  SOCKET connect_remote(const char* statement);

  int _send_msg_to_collector() {
    return chunks.drainOutWithPipe(
        std::bind(&TransLayer::_do_write_data, this, std::placeholders::_1, std::placeholders::_2));
  }

  void _reset_remote() {
    if (c_fd > 0) {
      pp_trace("reset peer:%d", c_fd);
      close_socket(c_fd);
      c_fd = -1;
      this->_state = 0;
    }

    if (statusChangedCallback_) {
      statusChangedCallback_(E_OFFLINE);
    }

    chunks.resetChunks();
  }

  int _do_write_data(const char* data, uint32_t length) {
    const char* buf = data;
    uint32_t buf_ofs = 0;
    while (buf_ofs < length) {
#if defined(__APPLE__) || defined(_WIN32)
      int ret = send(c_fd, buf + buf_ofs, length - buf_ofs, 0);
#else
      ssize_t ret = send(c_fd, buf + buf_ofs, length - buf_ofs, MSG_NOSIGNAL);
#endif
      if (ret > 0) {
        buf_ofs += (uint32_t)ret;
        pp_trace("fd %d send size %ld", c_fd, ret);
      } else if (ret == -1) {
        if (nonblock_error(err_code())) {
          this->_state |= S_WRITING;
          return buf_ofs;
        }
        pp_trace("_do_write_data@%d send data error:(%d) fd:(%d)", __LINE__, err_code(), c_fd);
        return -1;
      } else {
        pp_trace("_do_write_data@%d send data return 0 error:(%d) fd:(%d)", __LINE__, err_code(),
                 c_fd);
        return -1;
      }
    }
    // current task is done
    this->_state &= (~S_WRITING);
    return length;
  }

  int recvByteStream() {
    int next_size = 0;
    while (next_size < IN_MSG_BUF_SIZE) {
      int ret = recv(c_fd, in_buf + next_size, IN_MSG_BUF_SIZE - next_size, 0);
      if (ret > 0) {
        int total = ret + next_size;
        int msg_offset = HandleMsgStream(in_buf, total);
        if (msg_offset < total) {
          next_size = total - msg_offset;
          memcpy(in_buf, in_buf + msg_offset, next_size);
        } else {
          next_size = 0;
        }
      } else if (ret == 0) {
        pp_trace("server closed. error:%d", err_code());
        return -1;
      } else {
        if (nonblock_error(err_code())) {
          return 0;
        } else {
          pp_trace("recv failed. error:%d", err_code());
          return -1;
        }
      }
    }
    pp_trace("recv buf full,maybe a bug");
    return 0;
  }

  int HandleMsgStream(const char* buf, int len) {
    int offset = 0;
    while (offset + 8 <= len) {
      Header* header = (Header*)buf;

      int body_len = (int)ntohl(header->length);

      if (8 + body_len > len) {
        return offset;
      }

      uint32_t type = ntohl(header->type);
      auto has = msgRouteMap_.find(type);
      if (has != msgRouteMap_.end()) {
        msgRouteMap_[type](type, buf + 8, len - 8);
      } else {
        pp_trace("unsupported message type:%d from server", type);
      }
      offset += (8 + body_len);
    }
    return offset;
  }

private:
  void ResetMsgHandler() {
    statusChangedCallback_ = nullptr;
    msgRouteMap_.clear();
  }

private:
  const std::string& co_host;
  Chunks chunks;
  int32_t _state;
  char in_buf[IN_MSG_BUF_SIZE] = {0};
  std::function<void(int)> statusChangedCallback_;
  RouterMsgMap msgRouteMap_;
  const static char* UNIX_SOCKET;
  const static char* TCP_SOCKET;
  time_t lastConnectTime;

public:
  SOCKET c_fd;
};
using TransLayerPtr = std::unique_ptr<TransLayer>;
} // namespace ConnectionPool

#endif /* INCLUDE_PPTRANSLAYER_H_ */
