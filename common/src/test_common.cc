#include "gtest/gtest.h"
// #include <locale.h>
// #include <regex.h>
#include <thread>
#include <chrono>
#include "common.h"
#include "json/value.h"

using namespace testing;
std::string ouputMsg;
void cc_log_error_cb(const char* msg) { ouputMsg = msg; }

TEST(common, uid_all_in_one) {

  pinpoint_set_agent("tcp:127.0.0.1:9999", 7000, -1, 7000);

  int64_t startId = generate_unique_id();
  generate_unique_id();
  generate_unique_id();
  generate_unique_id();
  EXPECT_EQ(generate_unique_id(), 4 + startId);
}

TEST(common, start_end_trace) {
  register_span_handler(cc_log_error_cb);
  NodeID id = pinpoint_start_trace(E_ROOT_NODE);
  mark_current_trace_status(id, E_OFFLINE);
  EXPECT_EQ(pinpoint_trace_is_root(id), 1);
  id = pinpoint_start_trace(id);
  EXPECT_EQ(pinpoint_trace_is_root(id), 0);
  mark_current_trace_status(id, E_OFFLINE);
  EXPECT_EQ(pinpoint_trace_is_root(NodeID(-1023)), -1);
  EXPECT_EQ(pinpoint_trace_is_root(NodeID(1023)), -1);
  EXPECT_EQ(pinpoint_trace_is_root(NodeID(0)), -1);
  id = pinpoint_start_trace(id);

  mark_current_trace_status(id, E_TRACE_PASS);
  catch_error(id, "sdfasfas", "fsafdsfasd", 234);
  id = pinpoint_end_trace(id);

  id = pinpoint_end_trace(id);

  id = pinpoint_end_trace(id);
  EXPECT_EQ(id, 0);
  mark_current_trace_status(-1024, E_TRACE_BLOCK);
  mark_current_trace_status(1024, E_TRACE_BLOCK);
  catch_error(-1024, "sdfasfas", "fsafdsfasd", 234);
  catch_error(0, "sdfasfas", "fsafdsfasd", 234);
  EXPECT_TRUE(ouputMsg.find("ERR") != std::string::npos);
  // pp_trace("%s", ouputMsg.c_str());
}

TEST(common, context_check) {
  pinpoint_set_agent("tcp:127.0.0.1:9999", 7000, -1, 7000);

  NodeID id = pinpoint_start_trace(E_ROOT_NODE);
  std::string str = "fdafadf";
  pinpoint_add_clues(id, "fasdfas", str.c_str(), E_LOC_CURRENT);
  pinpoint_add_clue(id, "fasdfas", str.c_str(), E_LOC_CURRENT);
  pinpoint_add_clue(id, "fasdfas", str.c_str(), E_LOC_CURRENT);
  pinpoint_add_clue(id, "fasdfas", str.c_str(), E_LOC_CURRENT);

  id = pinpoint_start_trace(id);
  pinpoint_add_clue(id, "global", str.c_str(), E_LOC_ROOT);
  id = pinpoint_end_trace(id);
  str.clear();
  str = "fadfaffadf";
  pinpoint_set_context_key(id, "adfadf", str.c_str());
  str += "35486we32";
  pinpoint_set_context_key(id, "adfadf23", str.c_str());
  str.clear();
  char buf[1024] = {0};
  pinpoint_get_context_key(id, "adfadf23", buf, 1024);

  EXPECT_STREQ(buf, "fadfaffadf35486we32");
  pinpoint_get_context_key(id, "adfadf", buf, 1024);

  EXPECT_STREQ(buf, "fadfaffadf");

  pinpoint_set_context_key(id, "adfadf23", "fadfaffadf");

  pinpoint_set_context_key(id, ":interl", "abc");
  pinpoint_add_clue(id, ":internal", "abc", E_LOC_CURRENT);
  pinpoint_add_clue(id, ":internal-1", "abc", E_LOC_ROOT);

  pinpoint_add_clues(id, ":internal-abc", "abc", E_LOC_CURRENT);
  pinpoint_add_clues(id, ":internal-abc-1", "abc", E_LOC_ROOT);

  char outBuf[128] = {0};
  // no limitation on context key
  EXPECT_EQ(pinpoint_get_context_key(id, ":interl", outBuf, sizeof(outBuf)), 3);

  pinpoint_end_trace(id);
}

TEST(common, error_checking) {
  pinpoint_set_agent("tcp:127.0.0.1:9999", 7000, -1, 7000);

  NodeID id = pinpoint_start_trace(E_ROOT_NODE);
  EXPECT_EQ(pinpoint_trace_is_root(id), 1);
  id = pinpoint_end_trace(id);
  EXPECT_EQ(id, 0);

  id = pinpoint_start_trace(NodeID(1280));
  EXPECT_EQ(id, E_INVALID_NODE);
}

static void test_per_thread_id_odd() {
  pinpoint_set_agent("tcp:127.0.0.1:9999", 7000, -1, 7000);

  NodeID id = pinpoint_get_per_thread_id();
  EXPECT_EQ(id, 0);
  id = NodeID(1);
  for (int i = 1; i < 10000; i++) {
    int index = (int)id;
    index += 2;
    id = NodeID(index);
    pinpoint_update_per_thread_id(id);
    std::this_thread::sleep_for(std::chrono::microseconds(1));
    EXPECT_EQ(pinpoint_get_per_thread_id(), i * 2 + 1);
  }
}

static void test_per_thread_id_even() {
  NodeID id = pinpoint_get_per_thread_id();
  EXPECT_EQ(id, 0);

  for (int i = 1; i < 10000; i++) {
    int index = (int)id;
    index += 2;
    id = (NodeID)index;

    pinpoint_update_per_thread_id(id);
    std::this_thread::sleep_for(std::chrono::microseconds(1));
    EXPECT_EQ(pinpoint_get_per_thread_id(), i * 2);
  }
}

TEST(common, per_threadid) {
  pinpoint_set_agent("tcp:127.0.0.1:9999", 7000, -1, 7000);

  std::thread f1(test_per_thread_id_odd);
  std::thread f2(test_per_thread_id_even);
  f1.join();
  f2.join();
}

TEST(common, force_end_trace) {
  pinpoint_set_agent("tcp:127.0.0.1:9999", 0, -1, 7000);

  NodeID id = pinpoint_start_trace(E_ROOT_NODE);
  id = pinpoint_end_trace(id);
  id = pinpoint_start_trace(id);
  id = pinpoint_start_trace(id);
  id = pinpoint_start_trace(id);
  id = pinpoint_start_trace(id);
  id = pinpoint_end_trace(id);
  id = pinpoint_end_trace(id);
  EXPECT_NE(id, 0);
  int ret = pinpoint_force_end_trace(id, 300);
  EXPECT_EQ(ret, 0);
  id = NodeID(809999);
  pinpoint_force_end_trace(id, 300);
  pinpoint_force_end_trace(NodeID(-1235), 300);
  pinpoint_force_end_trace(NodeID(0), 300);
  pinpoint_force_end_trace(NodeID(1), 300);
  pinpoint_force_end_trace(NodeID(-1), 300);
}

TEST(common, version) { EXPECT_STREQ(pinpoint_agent_version(), AGENT_VERSION); }

TEST(common, invalid_host) {
  pinpoint_set_agent("tcp:0.0.0.0.0:9999", 7000, -1, 7000);
  NodeID id = pinpoint_start_trace(E_ROOT_NODE);
  id = pinpoint_end_trace(id);
}

TEST(common, pp_trace) {
  pp_trace("测试中文编码 \n");
  pp_trace("한국어 인코딩 테스트 \n");
  pp_trace("日本語エンコーディングをテストする \n");
  pp_trace("%s %s \n", "日本語エンコーディングをテストする ", "combine");
}

TEST(common, none_utf8) {
  // using Json = PP::AliasJson;
  AliasJson::Value value;
  value["a"] = "ä\xA9ü";
  pp_trace("%s ", value.toStyledString().c_str());
}