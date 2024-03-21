
#include <errno.h>
#include <functional>
#include <string>
#include <gtest/gtest.h>
#include "json/json.h"
#include <thread>
#include "SpanConnectionPool.h"
#include "sockets.h"
#include "header.h"
using namespace testing;
using ConnectionPool::SpanConnectionPool;
using ConnectionPool::TransLayer;
using ConnectionPool::TransLayerPtr;
namespace Json = AliasJson;

#define tcp_socket "127.0.0.1:5896"

bool run = true;

void sig_exit(int signm) {
  (void)signm;
  run = false;
}

void fake_server() {
  SOCKET fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#if defined(__linux__) || defined(__APPLE__)
  struct sockaddr_in sin;
  using SOCKET_ADDR_PTR = struct sockaddr*;
// typedef  SOCKET_ADDR_PTR;
#elif defined(_WIN32)
  SOCKADDR_IN sin;
  using SOCKET_ADDR_PTR = LPSOCKADDR;
#endif
  sin.sin_family = AF_INET;
  sin.sin_port = htons(5896);
  sin.sin_addr.s_addr = inet_addr("127.0.0.1");

  if (bind(fd, (SOCKET_ADDR_PTR)&sin, sizeof(sin)) == SOCKET_ERROR) {
    pp_trace("bind server socket failed:%s", strerror(errno));
    return;
  }
  listen(fd, 10);
  char buffer[1024] = {0};
  struct sockaddr_in client_addr;
  socklen_t addrlen = sizeof(client_addr);
  SOCKET cfd = accept(fd, (struct sockaddr*)&client_addr, &addrlen);
  if (cfd == INVALID_SOCKET) {
    pp_trace("accept failed with error=%d", err_code());
    return;
  }
  // assert(cfd != INVALID_SOCKET);
  pp_trace("recv cfd:%d", cfd);
  Json::Value agentInfo;

  agentInfo["time"] = 1234567;
  agentInfo["id"] = "test-app";
  agentInfo["name"] = "test-name";

  Json::FastWriter writer;
  std::string msg = writer.write(agentInfo);
  strcpy((char*)buffer + sizeof(Header), msg.c_str());

  int len = msg.length();
  Header* header = (Header*)buffer;
  header->type = htonl(RESPONSE_AGENT_INFO);
  header->length = htonl(len);
  int ret = send(cfd, buffer, len + sizeof(*header), 0);
  pp_trace("send [%d] [%d] %s ", cfd, ret, strerror(errno));
  recv(cfd, buffer, 1024, 0);
  close_socket(cfd);
}

void handle_agent_info(int type, const char* buf, size_t len) {
  printf("recv: %s", buf);
  EXPECT_EQ(type, RESPONSE_AGENT_INFO);
  Json::Value root;
  Json::Reader reader;
  reader.parse(buf, buf + len, root);
  EXPECT_EQ(root["time"].asInt64(), 1234567);
  EXPECT_STREQ(root["id"].asCString(), "test-app");
  EXPECT_STREQ(root["name"].asCString(), "test-name");
  run = false;
}

TEST(translayer, connect_remote) {
  TransLayer layer(tcp_socket);
  int ret = layer.connect_remote(tcp_socket);
  EXPECT_EQ(ret, -1);
}

TEST(translayer, tcp_socket_case) {
  std::thread server(fake_server);
  sleep(3);
  std::string remote = "tcp:" tcp_socket;
  TransLayer layer(remote);
  using namespace std::placeholders;
  layer.RegPeerMsgCallback(0, std::bind(handle_agent_info, _1, _2, _3));
  while (run) {
    layer.PoolEventOnce(3000);
  }
  std::string data = "msg-1918";
  layer.copy_into_send_buffer(data);
  layer.SyncSendAll(10);
  // waitpid(pid, 0, 0);
  server.join();
}

TEST(translayer, stream_socket_layer) {
  int fd = -1;
  fd = TransLayer::connect_stream_remote("www.naver.com:80");
  EXPECT_GT(fd, 2);
  close_socket(fd);

  fd = TransLayer::connect_stream_remote("172.217.175.68:80");
  EXPECT_GT(fd, 2);
  close_socket(fd);

  fd = TransLayer::connect_stream_remote("-.217.175.68:80");
  EXPECT_EQ(fd, -1);

  fd = TransLayer::connect_stream_remote("-.217.175.68:-80");
  EXPECT_EQ(fd, -1);

  fd = TransLayer::connect_stream_remote("-.217.175.68");
  EXPECT_EQ(fd, -1);

  fd = TransLayer::connect_stream_remote(":90");
  EXPECT_EQ(fd, -1);
}

TEST(translayer, API) {
  SpanConnectionPool _pool("unix:./pinpoint_test.sock", {{}});
  auto _conn = _pool.get();
  EXPECT_EQ(_conn->connect_stream_remote("-.217.175.68:-80"), -1);
  _pool.free(_conn);
}