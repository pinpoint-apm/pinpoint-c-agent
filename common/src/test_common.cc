#include "gtest/gtest.h"
// #include <locale.h>
// #include <regex.h>

#include <thread>
#include <chrono>
#include "pinpoint/common.h"
#include "json/value.h"
#include "json/reader.h"

using namespace testing;
std::string ouputMsg;
namespace Json = AliasJson;

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
  EXPECT_EQ(pinpoint_get_depth(E_ROOT_NODE), -1);
  NodeID id = pinpoint_start_trace(E_ROOT_NODE);
  EXPECT_EQ(pinpoint_get_depth(id), 0);
  change_trace_status(id, E_OFFLINE);
  EXPECT_EQ(pinpoint_trace_is_root(id), 1);
  id = pinpoint_start_trace(id);
  EXPECT_EQ(pinpoint_get_depth(id), 1);
  EXPECT_EQ(pinpoint_trace_is_root(id), 0);
  change_trace_status(id, E_OFFLINE);
  EXPECT_EQ(pinpoint_trace_is_root(NodeID(-1023)), -1);
  EXPECT_EQ(pinpoint_trace_is_root(NodeID(1023)), -1);
  EXPECT_EQ(pinpoint_trace_is_root(NodeID(0)), -1);
  id = pinpoint_start_trace(id);
  EXPECT_EQ(pinpoint_get_depth(id), 2);
  change_trace_status(id, E_TRACE_PASS);
  catch_error(id, "sdfasfas", "fsafdsfasd", 234);
  id = pinpoint_end_trace(id);

  id = pinpoint_end_trace(id);

  id = pinpoint_end_trace(id);
  EXPECT_EQ(id, 0);
  change_trace_status(-1024, E_TRACE_BLOCK);
  change_trace_status(1024, E_TRACE_BLOCK);
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

static std::string span;
static void capture(const char* msg) {
  pp_trace("capture:%s", msg);
  span = std::string(msg);
}

//./bin/TestCommon --gtest_filter=common.pinpoint_start_traceV1
TEST(common, pinpoint_start_traceV1) {
  pinpoint_set_agent("tcp:127.0.0.1:9999", 0, -1, 7000);
  register_span_handler(capture);
  NodeID root, child1;
  root = pinpoint_start_trace(E_ROOT_NODE);
  child1 = pinpoint_start_traceV1(root, "TraceMinTimeMs:23", nullptr);
  pinpoint_add_clue(child1, "name", "Take1sec", E_LOC_CURRENT);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  pinpoint_end_trace(child1);
  check_trace_limit(-1);
  check_trace_limit(0);
  check_trace_limit(time(nullptr));
  child1 = pinpoint_start_traceV1(root, "TraceOnlyException", nullptr);
  pinpoint_add_clue(child1, "name", "Exception", E_LOC_CURRENT);
  pinpoint_add_exception(child1, "xxxxxxxxx");
  pinpoint_end_trace(child1);

  child1 = pinpoint_start_traceV1(root, "TraceMinTimeMs:2000", nullptr);
  pinpoint_add_clue(child1, "name", "TraceMinTimeMs:2000", E_LOC_CURRENT);
  std::this_thread::sleep_for(std::chrono::seconds(1));

  // [ ] optional setting not ready
  // {
  //   NodeID child = pinpoint_start_traceV1(child1, "TraceMinTimeMs:23", nullptr);
  //   pinpoint_add_clue(child, "name", "childFromTraceMinTimeMs:23-1", E_LOC_CURRENT);
  //   pinpoint_end_trace(child);

  //   child = pinpoint_start_traceV1(child1, "TraceMinTimeMs:23", nullptr);
  //   pinpoint_add_clue(child, "name", "childFromTraceMinTimeMs:23-2", E_LOC_CURRENT);
  //   pinpoint_end_trace(child);

  //   child = pinpoint_start_traceV1(child1, "TraceMinTimeMs:23", nullptr);
  //   pinpoint_add_clue(child, "name", "childFromTraceMinTimeMs:23-3", E_LOC_CURRENT);

  //   pinpoint_end_trace(child);
  // }

  pinpoint_end_trace(child1);

  child1 = pinpoint_start_traceV1(root, "TraceOnlyException", nullptr);
  pinpoint_add_clue(child1, "name", "NoException", E_LOC_CURRENT);
  pinpoint_end_trace(child1);

  child1 = pinpoint_start_traceV1(root, "TraceMinTimeMs:-23", nullptr);
  pinpoint_add_clue(child1, "name", "TraceMinTimeMs:-23", E_LOC_CURRENT);
  pinpoint_end_trace(child1);

  pinpoint_end_trace(root);
  // pp_trace("span:%s", span.c_str());
  EXPECT_TRUE(span.length() > 0);
  EXPECT_TRUE(span.find("Take1sec") != span.npos);
  EXPECT_TRUE(span.find("Exception") != span.npos);
  // EXPECT_TRUE(span.find("TraceMinTimeMs:2000") == span.npos);
  // EXPECT_TRUE(span.find("NoException") == span.npos);

  // EXPECT_TRUE(span.find("childFromTraceMinTimeMs:23-3") == span.npos);
  // EXPECT_TRUE(span.find("childFromTraceMinTimeMs:23-2") == span.npos);
}

std::set<std::string> removed_keys = {":E", ":S"};
static void removeKeys(Json::Value& v) {
  Json::Value::Members mem = v.getMemberNames();
  for (auto iter = mem.begin(); iter != mem.end(); iter++) {
    if (removed_keys.find(*iter) != removed_keys.end()) {
      v.removeMember(*iter);
    }

    if (*iter == "event") {
      for (long i = 0; i < v[*iter].size(); i++) {
        removeKeys(v["event"][(int)i]);
      }
    }
  }

  return;
}

static bool check_span_order(std::string& i1, std::string& i2) {
  Json::Value v_i1, v_i2;
  Json::Reader reader;
  reader.parse(i1, v_i1, true);
  reader.parse(i2, v_i2, true);
  removeKeys(v_i1);
  removeKeys(v_i2);
  pp_trace("v_i1\r\n%s", v_i1.toStyledString().c_str());
  pp_trace("v_i2\r\n%s", v_i2.toStyledString().c_str());
  return v_i1.toStyledString() == v_i2.toStyledString();
}

TEST(common, call_order) {
  span.clear();
  register_span_handler(capture);
  NodeID root, child1, child2;
  root = pinpoint_start_trace(E_ROOT_NODE);
  pinpoint_add_clue(root, "name:", "root", E_LOC_CURRENT);
  child1 = pinpoint_start_trace(root);
  pinpoint_add_clue(child1, "name:", "child1->root", E_LOC_CURRENT);
  child2 = pinpoint_start_trace(child1);
  pinpoint_add_clue(child2, "name:", "child2->child1", E_LOC_CURRENT);
  child2 = pinpoint_start_trace(child1);
  pinpoint_add_clue(child2, "name:", "child3->child1", E_LOC_CURRENT);
  pinpoint_end_trace(child2);
  child2 = pinpoint_start_trace(child1);
  pinpoint_add_clue(child2, "name:", "child4->child1", E_LOC_CURRENT);
  pinpoint_end_trace(child2);
  child2 = pinpoint_start_trace(child1);
  pinpoint_add_clue(child2, "name:", "child5->child1", E_LOC_CURRENT);
  child2 = pinpoint_start_trace(child2);
  pinpoint_add_clue(child2, "name:", "child6->child5", E_LOC_CURRENT);

  pinpoint_end_trace(child2);
  pinpoint_end_trace(child1);
  pinpoint_end_trace(root);

  std::string exp =
      R"({":FT":7000,"event":[{":E":0,":S":0,":depth":1,":seq":0,"name:":"child1->root"},{":E":0,":S":0,":depth":2,":seq":1,"name:":"child2->child1"},{":E":0,":S":0,":depth":2,":seq":2,"name:":"child3->child1"},{":E":0,":S":0,":depth":2,":seq":3,"name:":"child4->child1"},{":E":0,":S":0,":depth":2,":seq":4,"name:":"child5->child1"},{":E":0,":S":0,":depth":3,":seq":5,"name:":"child6->child5"}],"name:":"root"})";
  printf("\n%s\n", span.c_str());
  EXPECT_TRUE(check_span_order(span, exp));
}
std::mutex cv_m;
std::condition_variable cv;
NodeID rootId = E_ROOT_NODE;
// note: as it known, there may leak some nodes
static void func() {
  std::unique_lock<std::mutex> lk(cv_m);
  cv.wait(lk);
  pinpoint_add_clues(rootId, "xxxx", "bbbbbbss", E_LOC_CURRENT);
  pinpoint_add_clue(rootId, "xxx", "bbbbbb", E_LOC_CURRENT);
  for (int i = 0; i < 100; ++i) {
    rootId = pinpoint_start_trace(rootId);
    pinpoint_set_context_key(rootId, "xxxx", "bbbbbb");
    std::this_thread::yield();
    char buf[1024] = {0};
    auto len = pinpoint_get_context_key(rootId, "xxxx", buf, 1024);
    assert(len > 0);
    std::cout << rootId << "read value:" << buf << std::endl;
    pinpoint_add_clues(rootId, "xxxx", "bbbbbbss", E_LOC_CURRENT);
    std::this_thread::yield();
    pinpoint_add_clue(rootId, "xxx", "bbbbbb", E_LOC_CURRENT);
    std::this_thread::yield();
    rootId = pinpoint_end_trace(rootId);
    std::this_thread::yield();
  }
  pinpoint_add_clues(rootId, "xxxx", "bbbbbbss", E_LOC_CURRENT);
  pinpoint_add_clue(rootId, "xxx", "bbbbbb", E_LOC_CURRENT);
}

TEST(common, multipleThread) {
  // no crash, works fine
  NodeID root = pinpoint_start_trace(E_ROOT_NODE);

  std::vector<std::thread> threads;

  for (int i = 0; i < 10; i++) {
    threads.push_back(std::thread(func));
  }

  std::this_thread::sleep_for(std::chrono::seconds(2));
  cv.notify_all();

  for (int i = 0; i < 10; i++) {
    threads[i].join();
  }
  pinpoint_end_trace(root);
  pinpoint_end_trace(root);
  // EXPECT_TRUE(PoolManager::getInstance().NoNodeLeak());
}

TEST(common, sequenceId) {
  NodeID root = pinpoint_start_trace(E_ROOT_NODE);
  NodeID child_01 = pinpoint_start_trace(root);
  NodeID child_02 = pinpoint_start_trace(child_01);
  EXPECT_EQ(pinpoint_get_sequence_id(child_01), 0);
  EXPECT_EQ(pinpoint_get_sequence_id(child_02), 1);
  EXPECT_EQ(pinpoint_get_sequence_id(root), 0);
  NodeID child_03 = pinpoint_start_trace(child_02);
  EXPECT_EQ(pinpoint_get_sequence_id(child_03), 2);
  pinpoint_end_trace(root);
  EXPECT_EQ(pinpoint_get_sequence_id(child_03), -1);
  show_status();
}

static void simple_call() {
  NodeID root = pinpoint_start_trace(E_ROOT_NODE);
  NodeID child_01 = pinpoint_start_trace(root);
  NodeID child_02 = pinpoint_start_trace(child_01);
  EXPECT_EQ(pinpoint_get_sequence_id(child_01), 0);
  EXPECT_EQ(pinpoint_get_sequence_id(child_02), 1);
  EXPECT_EQ(pinpoint_get_sequence_id(root), 0);
  NodeID child_03 = pinpoint_start_trace(child_02);
  EXPECT_EQ(pinpoint_get_sequence_id(child_03), 2);
  pinpoint_end_trace(root);

  show_status();
}

TEST(common, thread) {
  std::thread t1(simple_call);
  std::thread t2(simple_call);
  std::thread t3(simple_call);
  t1.join();
  t2.join();
  t3.join();
}

TEST(common, async_ctx) {
  NodeID root = pinpoint_start_trace(E_ROOT_NODE);
  NodeID child_01 = pinpoint_start_trace(root);
  NodeID child_02 = pinpoint_start_trace(child_01);
  EXPECT_EQ(pinpoint_get_sequence_id(child_01), 0);
  EXPECT_EQ(pinpoint_get_sequence_id(child_02), 1);
  EXPECT_EQ(pinpoint_get_sequence_id(root), 0);
  NodeID child_03 = pinpoint_start_trace(child_02);
  EXPECT_EQ(pinpoint_get_sequence_id(child_03), 2);

  pinpoint_set_async_ctx(root, 256, 0);
  pinpoint_end_trace(root);
  pinpoint_set_async_ctx(1024, 256, 0);
}