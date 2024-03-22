#include <gtest/gtest.h>
#include "common.h"
#include "ConnectionPool/sockets.h"
#include "header.h"
void log_msg_cb_(char* msg) { printf("%s", msg); }
int main(int argc, char** argv) {
  socket_init();
  pinpoint_set_agent("tcp:127.0.0.1:9999", 0, -1, 7000);
  register_logging_cb(log_msg_cb_, 1);
  ::testing::InitGoogleTest(&argc, argv);
  // initial_crash_collection();
  auto ret = RUN_ALL_TESTS();
  sock_cleanup();
  pinpoint_stop_agent();
  return ret;
}