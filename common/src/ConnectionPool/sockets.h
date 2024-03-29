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
#pragma once
// porting from https://learn.microsoft.com/en-us/windows/win32/winsock/complete-client-code
#if defined(__linux__) || defined(__APPLE__)
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netdb.h>
typedef int SOCKET;
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

inline void set_socket_none_block(SOCKET sock) {
  fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0) | O_NONBLOCK);
}

inline int close_socket(SOCKET fd) { return close(fd); }
inline int socket_init() { return 0; }
inline void sock_cleanup() {}
inline int err_code() { return errno; }
inline bool nonblock_error(int err_code) {
  return err_code == EAGAIN || err_code == EWOULDBLOCK || err_code == EINTR ||
         err_code == EINPROGRESS;
}
#elif defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "AdvApi32.lib")
typedef SOCKET SOCKET;

inline void set_socket_none_block(SOCKET sock) {
  u_long mode = 1;
  ioctlsocket(sock, FIONBIO, &mode);
}

inline int close_socket(SOCKET fd) { return closesocket(fd); }

inline int err_code() { return WSAGetLastError(); }

inline int socket_init() {
  WSADATA wsaData;
  return WSAStartup(MAKEWORD(2, 2), &wsaData);
}
inline void sock_cleanup() { WSACleanup(); }
inline bool nonblock_error(int err_code) { return err_code == WSAEWOULDBLOCK; }
#endif