
#include <errno.h>
#include <functional>
#include <string>
#include <gtest/gtest.h>
#include "json/json.h"
#include <thread>
#include <vector>
#include "SpanConnectionPool.h"
#include "sockets.h"
#include "header.h"
using namespace testing;
using ConnectionPool::SpanConnectionPool;
using ConnectionPool::TransLayer;
using ConnectionPool::TransLayerPtr;
namespace Json = AliasJson;

static std::string make_frame(const std::string& body) {
  Header header = {htonl(RESPONSE_AGENT_INFO), htonl(body.size())};
  return std::string(reinterpret_cast<const char*>(&header), sizeof(header)) + body;
}

TEST(translayer, coalesced_frames_have_exact_bodies) {
  const std::string host = "tcp:127.0.0.1:1";
  TransLayer layer(host);
  std::vector<std::string> received;
  layer.RegPeerMsgCallback(RESPONSE_AGENT_INFO, [&](int, const char* data, size_t length) {
    received.emplace_back(data, length);
    return 0;
  });
  std::string frames = make_frame("first") + make_frame("second");
  EXPECT_EQ(static_cast<int>(frames.size()), layer.HandleMsgStream(frames.data(), frames.size()));
  ASSERT_EQ(2u, received.size());
  EXPECT_EQ("first", received[0]);
  EXPECT_EQ("second", received[1]);
}

TEST(translayer, rejects_oversized_frame_lengths) {
  const std::string host = "tcp:127.0.0.1:1";
  TransLayer layer(host);
  for (uint32_t length : {uint32_t(IN_MSG_BUF_SIZE), UINT32_MAX, UINT32_MAX - 7}) {
    Header header = {htonl(RESPONSE_AGENT_INFO), htonl(length)};
    EXPECT_EQ(-1, layer.HandleMsgStream(reinterpret_cast<const char*>(&header), sizeof(header)));
  }
}

#if defined(__linux__) || defined(__APPLE__)
TEST(translayer, fragmented_frames_survive_would_block) {
  const std::string host = "tcp:127.0.0.1:1";
  const std::string packet = make_frame("split-message");
  for (size_t split = 1; split < packet.size(); ++split) {
    TransLayer layer(host);
    int pair[2];
    ASSERT_EQ(0, socketpair(AF_UNIX, SOCK_STREAM, 0, pair));
    layer.c_fd = pair[0];
    set_socket_none_block(layer.c_fd);
    std::vector<std::string> received;
    layer.RegPeerMsgCallback(RESPONSE_AGENT_INFO, [&](int, const char* data, size_t length) {
      received.emplace_back(data, length);
      return 0;
    });
    EXPECT_EQ(static_cast<ssize_t>(split), send(pair[1], packet.data(), split, 0));
    EXPECT_EQ(0, layer.recvByteStream());
    EXPECT_TRUE(received.empty());
    EXPECT_EQ(static_cast<ssize_t>(packet.size() - split),
              send(pair[1], packet.data() + split, packet.size() - split, 0));
    EXPECT_EQ(0, layer.recvByteStream());
    EXPECT_EQ(std::vector<std::string>{"split-message"}, received);
    close_socket(pair[1]);
  }
}

TEST(translayer, complete_frame_followed_by_partial_frame) {
  const std::string host = "tcp:127.0.0.1:1";
  TransLayer layer(host);
  int pair[2];
  ASSERT_EQ(0, socketpair(AF_UNIX, SOCK_STREAM, 0, pair));
  layer.c_fd = pair[0];
  set_socket_none_block(layer.c_fd);
  std::vector<std::string> received;
  layer.RegPeerMsgCallback(RESPONSE_AGENT_INFO, [&](int, const char* data, size_t length) {
    received.emplace_back(data, length);
    return 0;
  });
  const std::string second = make_frame(std::string(100, 'b'));
  const std::string batch = make_frame("a") + second.substr(0, 80);
  EXPECT_EQ(static_cast<ssize_t>(batch.size()), send(pair[1], batch.data(), batch.size(), 0));
  EXPECT_EQ(0, layer.recvByteStream());
  EXPECT_EQ(std::vector<std::string>{"a"}, received);
  EXPECT_EQ(static_cast<ssize_t>(second.size() - 80),
            send(pair[1], second.data() + 80, second.size() - 80, 0));
  EXPECT_EQ(0, layer.recvByteStream());
  const std::vector<std::string> expected = {"a", std::string(100, 'b')};
  EXPECT_EQ(expected, received);
  close_socket(pair[1]);
}

TEST(translayer, high_descriptor_is_rejected_before_fd_set) {
  const std::string host = "tcp:127.0.0.1:1";
  TransLayer layer(host);
  int pair[2];
  ASSERT_EQ(0, socketpair(AF_UNIX, SOCK_STREAM, 0, pair));
  layer.c_fd = fcntl(pair[0], F_DUPFD, FD_SETSIZE);
  close_socket(pair[0]);
  close_socket(pair[1]);
  if (layer.c_fd == INVALID_SOCKET) {
    GTEST_SKIP() << "process descriptor limit is below FD_SETSIZE";
  }
  EXPECT_EQ(static_cast<size_t>(-1), layer.PoolEventOnce(0));
  EXPECT_EQ(INVALID_SOCKET, layer.c_fd);
}

TEST(translayer, reconnect_discards_previous_partial_frame) {
  const std::string host = "tcp:127.0.0.1:1";
  TransLayer layer(host);
  std::vector<std::string> received;
  layer.RegPeerMsgCallback(RESPONSE_AGENT_INFO, [&](int, const char* data, size_t length) {
    received.emplace_back(data, length);
    return 0;
  });
  const std::string packet = make_frame("new-connection");
  int pair[2];
  ASSERT_EQ(0, socketpair(AF_UNIX, SOCK_STREAM, 0, pair));
  layer.c_fd = pair[0];
  set_socket_none_block(layer.c_fd);
  EXPECT_EQ(4, send(pair[1], packet.data(), 4, 0));
  EXPECT_EQ(0, layer.recvByteStream());
  layer._reset_remote();
  close_socket(pair[1]);

  ASSERT_EQ(0, socketpair(AF_UNIX, SOCK_STREAM, 0, pair));
  layer.c_fd = pair[0];
  set_socket_none_block(layer.c_fd);
  EXPECT_EQ(static_cast<ssize_t>(packet.size()), send(pair[1], packet.data(), packet.size(), 0));
  EXPECT_EQ(0, layer.recvByteStream());
  EXPECT_EQ(std::vector<std::string>{"new-connection"}, received);
  close_socket(pair[1]);
}
#endif

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

int handle_agent_info(int type, const char* buf, size_t len) {
  printf("recv: %s", buf);
  EXPECT_EQ(type, RESPONSE_AGENT_INFO);
  Json::Value root;
  Json::Reader reader;
  reader.parse(buf, buf + len, root);
  EXPECT_EQ(root["time"].asInt64(), 1234567);
  EXPECT_STREQ(root["id"].asCString(), "test-app");
  EXPECT_STREQ(root["name"].asCString(), "test-name");
  run = false;
  return 0;
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
