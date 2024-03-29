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
#include "TransLayer.h"
#include "ConnectionPool/sockets.h"
#include "common.h"
#include <cctype>
#include <ctime>
namespace ConnectionPool {

int strcasecmp_(const char* s1, const char* s2) {
  const char* p_s1 = s1;
  const char* p_s2 = s2;
  while (*p_s1 != '\0' && *p_s2 != '\0') {
    int p1 = static_cast<int>(*p_s1);
    int p2 = static_cast<int>(*p_s2);
    if (std::toupper(p1) != std::toupper(p2)) {
      return p1 - p2;
    } else {
      p_s1++;
      p_s2++;
    }
  }
  return *p_s2 == '\0' ? 0 : -1;
}

/**
 * remote: localhost:port
 */
SOCKET TransLayer::connect_stream_remote(const char* remote) {
  pp_trace("agent try to connect:(%s)", remote);
  int offset = (int)strlen(remote) - 1;
  while (remote[offset] != ':') {
    offset--;
    if (offset <= 0) {
      pp_trace("get an invalid remote %s", remote);
      return -1;
    }
  }

  std::string hostname(remote, offset);
  const char* port_str = remote + offset + 1;

  struct addrinfo hints;
  struct addrinfo *result, *rp;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;     /* Allow IPv4 or IPv6 */
  hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
  // hints.ai_flags = 0;
  hints.ai_protocol = IPPROTO_TCP; /* Any protocol */

  int s = 0;
  s = getaddrinfo(hostname.c_str(), port_str, &hints, &result);
  if (s != 0) {
    pp_trace("getaddrinfo failed: hostname:%s msg:%s ", hostname.c_str(), gai_strerror(s));
    return -1;
  }
  SOCKET sfd = INVALID_SOCKET;
  for (rp = result; rp != NULL; rp = rp->ai_next) {
    sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

    // mark it as nonblock
    set_socket_none_block(sfd);
    struct linger fd_linger = {1, 1}; // open and 1 second
#ifdef _WIN32
    setsockopt(sfd, SOL_SOCKET, SO_LINGER, (char*)&fd_linger, sizeof(fd_linger));
#else
    setsockopt(sfd, SOL_SOCKET, SO_LINGER, &fd_linger, sizeof(fd_linger));
#endif
    if (sfd == -1)
      continue;
    int ret = connect(sfd, rp->ai_addr, rp->ai_addrlen);
    if (ret == 0) {
      break;
    } else if (ret == -1) {
      if (nonblock_error(err_code())) {
        break;
      } else {
        pp_trace("connect failed. error=%d", err_code());
      }
    }
    close_socket(sfd);
    sfd = INVALID_SOCKET;
  }
  freeaddrinfo(result);

  return sfd;
}
#ifdef __linux__
int TransLayer::connect_unix_remote(const char* remote) {
  pp_trace("agent try to connect:(%s)", remote);
  SOCKET fd = -1;
#if defined(__APPLE__)
  struct sockaddr_un u_sock = {0, 0, {0}};
#else
  struct sockaddr_un u_sock = {0, {0}};
#endif
  struct linger fd_linger = {1, 1}; // open and 1 second
  if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
    pp_trace(" get socket error error_code = %d", err_code());
    goto ERR_NETWORK;
  }

  u_sock.sun_family = AF_UNIX;
  strncpy(u_sock.sun_path, remote, sizeof(u_sock.sun_path) - 1);

  // mark fd as non blocking
  fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);

  setsockopt(fd, SOL_SOCKET, SO_LINGER, &fd_linger, sizeof(fd_linger));

  if (connect(fd, (struct sockaddr*)&u_sock, sizeof(u_sock)) != 0) {
    if (err_code() != EALREADY || err_code() != EINPROGRESS) {
      pp_trace("connect:(%s) failed error_code: %d", remote, err_code());
      goto ERR_NETWORK;
    }
  }
  pp_trace("connected to %s", remote);

  return fd;

ERR_NETWORK:
  if (fd > 0) {
    close(fd);
  }

  return -1;
}
#endif
SOCKET TransLayer::connect_remote(const char* statement) {
  SOCKET fd = -1;
  int socket_type_offset = 0;
  // const char* substring = NULL;
  if (statement == NULL || statement[0] == '\0') {
    goto E_INVALID_FORMAT;
  }

  // check last connect time
  if (std::time(nullptr) < this->lastConnectTime + RECONNECT_TIME_SEC) {
    goto RECONNECT_WAITING;
  } else {
    this->lastConnectTime = std::time(nullptr);
  }

  /// unix
#ifdef __linux__
  socket_type_offset = strcasecmp_(statement, UNIX_SOCKET);
  if (socket_type_offset == 0) {
    // sizeof = len +1, so substring -> /tmp/collector.sock
    // substring = substring + strlen(UNIX_SOCKET);
    fd = connect_unix_remote(statement + strlen(TCP_SOCKET));
    c_fd = fd;
    goto DONE;
  }
#endif
  ///  tcp tcp:localhost:port
  socket_type_offset = strcasecmp_(statement, TCP_SOCKET);
  if (socket_type_offset == 0) {
    fd = TransLayer::connect_stream_remote(statement + strlen(TCP_SOCKET));
    c_fd = fd;
    goto DONE;
  }

E_INVALID_FORMAT:
  pp_trace("remote is not valid:%s", statement);
  return -1;
RECONNECT_WAITING:
  return -1;
DONE:
  this->_state |= (S_ERROR | S_READING | S_WRITING);
  return fd;
}

size_t TransLayer::PoolEventOnce(uint32_t timeout) {
  if (c_fd == -1) {
    connect_remote(this->co_host.c_str());
    if (c_fd == -1) {
      return -1;
    }
  }

  SOCKET fd = c_fd;
  fd_set wfds, efds, rfds;
  FD_ZERO(&efds);

  FD_ZERO(&wfds);
  FD_ZERO(&rfds);

  // if (this->_state & S_ERROR) {
  FD_SET(fd, &efds);
  // }

  if (this->_state & S_WRITING) {
    FD_SET(fd, &wfds);
  }

  // if (this->_state & S_READING) {
  FD_SET(fd, &rfds);
  // }

  struct timeval tv = {timeout / 1000, (int)timeout % 1000};

  int retval = select(fd + 1, &rfds, &wfds, &efds, &tv);
  if (retval == SOCKET_ERROR) {
    // it helped address the macosx issue
    pp_trace("select return error_code=%d", err_code());
    return -1;
  } else if (retval > 0) {
    if ((this->_state & S_ERROR) && FD_ISSET(fd, &efds)) {
      pp_trace("error event");
      goto ERR_NETWORK;
    }

    if ((this->_state & S_WRITING) && FD_ISSET(fd, &wfds)) {
      pp_trace("write event");
      if (_send_msg_to_collector() == -1) {
        goto ERR_NETWORK;
      }
    }

    if ((this->_state & S_READING) && FD_ISSET(fd, &rfds)) {
      pp_trace("read event");
      if (recvByteStream() == -1) {
        pp_trace("recv_msg_from_collector error");
        goto ERR_NETWORK;
      }
    }
  }
  // timeout
  return 0;

ERR_NETWORK:

  _reset_remote();

  connect_remote(this->co_host.c_str());
  return -1;
}

bool TransLayer::copy_into_send_buffer(const std::string& data) {
  Header header;
  header.length = htonl(data.size());
  header.type = htonl(REQ_UPDATE_SPAN);

  if (this->chunks.checkCapacity(sizeof(header) + data.size()) == false) {
    pp_trace("Send buffer is full. size:[%ld]", data.size() + sizeof(header));
    return false;
  }
  // copy header
  this->chunks.copyDataIntoChunks((const char*)&header, sizeof(header));
  // copy body
  this->chunks.copyDataIntoChunks(data.data(), data.size());
  // enable write event
  this->_state |= S_WRITING;
  return true;
}

const char* TransLayer::UNIX_SOCKET = "unix:";
const char* TransLayer::TCP_SOCKET = "tcp:";
} // namespace ConnectionPool