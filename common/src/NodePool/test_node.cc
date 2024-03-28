#include "common.h"
#include "json/value.h"
#include <gtest/gtest.h>
#include "header.h"
#include "NodePool/PoolManager.h"
#include <condition_variable>
#include <thread>
#include <chrono>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iostream>

using PP::NodePool::PoolManager;
using PP::NodePool::TraceNode;
using PP::NodePool::WrapperTraceNodePtr;
using namespace testing;
namespace Json = AliasJson;
PoolManager nodePool_;
std::mutex cv_m;
std::condition_variable cv;
NodeID rootId = E_ROOT_NODE;

// note: as it known, there may leak some node
void func() {
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

TEST(node, multipleThread) {
  // no crash, works fine
  NodeID root = pinpoint_start_trace(E_ROOT_NODE);

  std::vector<std::thread> threads;

  for (int i = 0; i < 10; i++) {
    threads.push_back(std::thread(func));
  }

  sleep(2);
  cv.notify_all();

  for (int i = 0; i < 10; i++) {
    threads[i].join();
  }
  pinpoint_end_trace(root);
  pinpoint_end_trace(root);
  // EXPECT_TRUE(PoolManager::getInstance().NoNodeLeak());
}

void test_opt(TraceNode& node, const char* opt, ...) {
  va_list args;
  va_start(args, opt);
  node.setOpt(opt, &args);
  va_end(args);
}

TEST(node, opt) {

  TraceNode& node = nodePool_.Take();

  test_opt(node, "TraceMinTimeMs:23", "TraceOnlyException", nullptr);

  node.cumulative_time = 22;
  EXPECT_FALSE(node.checkOpt());
  node.cumulative_time = 23;
  EXPECT_TRUE(node.checkOpt());

  node.cumulative_time = 0;
  node.set_exp_ = true;
  EXPECT_TRUE(node.checkOpt());
  node.set_exp_ = false;
  EXPECT_FALSE(node.checkOpt());

  nodePool_.ReturnNode(node);
  // EXPECT_TRUE(PoolManager::getInstance().NoNodeLeak());
}

static std::string span;
void capture(const char* msg) {
  pp_trace("capture:%s", msg);
  span = std::string(msg);
}
//./bin/TestCommon --gtest_filter=node.pinpoint_start_traceV1
TEST(node, pinpoint_start_traceV1) {
  pinpoint_set_agent("tcp:127.0.0.1:9999", 0, -1, 7000);
  register_span_handler(capture);
  NodeID root, child1;
  root = pinpoint_start_trace(E_ROOT_NODE);
  child1 = pinpoint_start_traceV1(root, "TraceMinTimeMs:23", nullptr);
  pinpoint_add_clue(child1, "name", "Take1sec", E_LOC_CURRENT);
  sleep(1);
  pinpoint_end_trace(child1);
  check_tracelimit(-1);
  check_tracelimit(0);
  check_tracelimit(time(nullptr));
  child1 = pinpoint_start_traceV1(root, "TraceOnlyException", nullptr);
  pinpoint_add_clue(child1, "name", "Exception", E_LOC_CURRENT);
  pinpoint_add_exception(child1, "xxxxxxxxx");
  pinpoint_end_trace(child1);

  child1 = pinpoint_start_traceV1(root, "TraceMinTimeMs:2000", nullptr);
  pinpoint_add_clue(child1, "name", "TraceMinTimeMs:2000", E_LOC_CURRENT);
  sleep(1);

  {
    NodeID child = pinpoint_start_traceV1(child1, "TraceMinTimeMs:23", nullptr);
    pinpoint_add_clue(child1, "name", "childFromTraceMinTimeMs:23-1", E_LOC_CURRENT);
    pinpoint_end_trace(child);
    child = pinpoint_start_traceV1(child1, "TraceMinTimeMs:23", nullptr);
    pinpoint_add_clue(child1, "name", "childFromTraceMinTimeMs:23-2", E_LOC_CURRENT);
    pinpoint_end_trace(child);
    child = pinpoint_start_traceV1(child1, "TraceMinTimeMs:23", nullptr);
    pinpoint_add_clue(child1, "name", "childFromTraceMinTimeMs:23-3", E_LOC_CURRENT);
    pinpoint_end_trace(child);
  }

  pinpoint_end_trace(child1);

  child1 = pinpoint_start_traceV1(root, "TraceOnlyException", nullptr);
  pinpoint_add_clue(child1, "name", "NoException", E_LOC_CURRENT);
  pinpoint_end_trace(child1);

  child1 = pinpoint_start_traceV1(root, "TraceMinTimeMs:-23", nullptr);
  pinpoint_add_clue(child1, "name", "TraceMinTimeMs:-23", E_LOC_CURRENT);
  pinpoint_end_trace(child1);

  pinpoint_end_trace(root);
  pp_trace("span:%s", span.c_str());
  EXPECT_TRUE(span.length() > 0);
  EXPECT_TRUE(span.find("Take1sec") != span.npos);
  EXPECT_TRUE(span.find("Exception") != span.npos);
  EXPECT_TRUE(span.find("TraceMinTimeMs:2000") == span.npos);
  EXPECT_TRUE(span.find("NoException") == span.npos);

  EXPECT_TRUE(span.find("childFromTraceMinTimeMs:23-3") == span.npos);
  EXPECT_TRUE(span.find("childFromTraceMinTimeMs:23-2") == span.npos);
}

TEST(node, leak_node) {
  auto count = nodePool_.freeNodesCount();
  NodeID root, child1, child2;
  root = pinpoint_start_trace(E_ROOT_NODE);
  child1 = pinpoint_start_trace(root);
  child2 = pinpoint_start_trace(child1);
  // root goes first
  pinpoint_end_trace(root);

  pinpoint_end_trace(child1);
  pinpoint_end_trace(child2);

  EXPECT_EQ(count, nodePool_.freeNodesCount());
  show_status();
}
std::set<std::string> removed_keys = {":E", ":S"};
void Remove(Json::Value& v) {
  Json::Value::Members mem = v.getMemberNames();
  for (auto iter = mem.begin(); iter != mem.end(); iter++) {
    if (removed_keys.find(*iter) != removed_keys.end()) {
      v.removeMember(*iter);
    }

    if (*iter == "calls") {
      for (long i = 0; i < v[*iter].size(); i++) {
        Remove(v["calls"][(int)i]);
      }
    }
  }

  return;
}

bool check_span_order(std::string& i1, std::string& i2) {
  Json::Value v_i1, v_i2;
  Json::Reader reader;
  reader.parse(i1, v_i1, false);
  reader.parse(i2, v_i2, false);
  Remove(v_i1);
  Remove(v_i2);
  pp_trace("%s", v_i1.toStyledString().c_str());
  pp_trace("%s", v_i2.toStyledString().c_str());
  return v_i1.toStyledString() == v_i2.toStyledString();
}

TEST(node, call_order) {
  register_span_handler(capture);
  NodeID root, child1, child2;
  root = pinpoint_start_trace(E_ROOT_NODE);
  pinpoint_add_clue(root, "name:", "root", E_LOC_CURRENT);
  child1 = pinpoint_start_trace(root);
  pinpoint_add_clue(child1, "name:", "child1", E_LOC_CURRENT);
  child2 = pinpoint_start_trace(child1);
  pinpoint_add_clue(child2, "name:", "child2", E_LOC_CURRENT);
  child2 = pinpoint_start_trace(child1);
  pinpoint_add_clue(child2, "name:", "child3", E_LOC_CURRENT);
  pinpoint_end_trace(child2);
  child2 = pinpoint_start_trace(child1);
  pinpoint_add_clue(child2, "name:", "child4", E_LOC_CURRENT);
  pinpoint_end_trace(child2);
  child2 = pinpoint_start_trace(child1);
  pinpoint_add_clue(child2, "name:", "child5", E_LOC_CURRENT);
  pinpoint_end_trace(child2);
  pinpoint_end_trace(child1);
  pinpoint_end_trace(root);

  std::string exp =
      R"({":E":0,":FT":7000,":S":1710408777521,"calls":[{":E":0,":S":0,"calls":[{"name:":"child2"},{":E":0,":S":0,"name:":"child3"},{":E":0,":S":0,"name:":"child4"},{":E":0,":S":0,"name:":"child5"}],"name:":"child1"}],"name:":"root"})";
  EXPECT_TRUE(check_span_order(span, exp));
}

TEST(node, tons_of_nodes_01) {
  pinpoint_set_agent("tcp:127.0.0.1:9999", 7000, -1, 7000);
  // auto count = usedNode();
  NodeID root = pinpoint_start_trace(E_ROOT_NODE);
  for (int i = 0; i < 1000; i++) {
    NodeID child1 = pinpoint_start_trace(root);
    pinpoint_end_trace(child1);
  }
  pinpoint_end_trace(root);

  // EXPECT_EQ(count, usedNode()); //);

  root = pinpoint_start_trace(E_ROOT_NODE);
  NodeID child1 = root;
  for (int i = 0; i < 1000; i++) {
    NodeID child = pinpoint_start_trace(child1);
    mark_current_trace_status(root, E_OFFLINE);
    pinpoint_end_trace(child);
    child1 = child;
  }
  mark_current_trace_status(root, E_TRACE_BLOCK);
  pinpoint_end_trace(root);
}
// HACK here reproduce a bug in win11, not found any solution
// unknown file: error: SEH exception with code 0xc00000fd thrown in the test body.
// static int loop = 0;
// int test_loop(Json::Value& value, int i) {
//   loop++;
//   if (i == 0) {
//     return 0;
//   }
//   printf("%d \n", loop);
//   if (i % 2) {
//     Json::Value v;
//     test_loop(v, i - 1);
//     value["calls"].append(v);
//   } else {
//     test_loop(value, i - 1);
//   }

//   if (i % 3) {
//     value["id"] = 2;
//   }
//   return 0;
// }

// TEST(node, test_json) {
//   // std::srand(std::time(nullptr));
//   // Json::Value pvalue;
//   // // for (int i = 0; i < 2000; i++) {
//   // test_loop(pvalue, 1000);
//   // // }
//   // printf("%s", pvalue.toStyledString().c_str());
//   char* p = (char*)10;
//   *p = 10;
// }
// HACK: window crash when nodes reached 1k
// solution: use pspanchunk protocol
TEST(node, tons_of_nodes_1k) {
  NodeID root = pinpoint_start_trace(E_ROOT_NODE);
  NodeID next = root;
  for (int i = 0; i < 1000; i++) {
    NodeID child = pinpoint_start_trace(next);
    pinpoint_add_clue(child, "id", std::to_string(child).c_str(), E_LOC_CURRENT);
    pinpoint_end_trace(child);

    if (i % 2 == 0) {
      next = child;
    }
  }
  pinpoint_end_trace(root);
  show_status();
}

TEST(node, tons_of_nodes_leak) {
  NodeID root, child_1, child_2;
  root = pinpoint_start_trace(E_ROOT_NODE);

  child_1 = pinpoint_start_trace(root);
  child_2 = pinpoint_start_trace(child_1);

  pinpoint_end_trace(child_2);

  pinpoint_end_trace(root);
  pinpoint_end_trace(child_1);
}

TEST(node, tons_of_nodes_free_all) {

  NodeID root, child_1, child_2;
  root = pinpoint_start_trace(E_ROOT_NODE);

  child_1 = pinpoint_start_trace(root);
  child_2 = pinpoint_start_trace(child_1);
  pinpoint_end_trace(child_2);

  pinpoint_end_trace(root);
  pinpoint_end_trace(child_1);
}
//./bin/TestCommon --gtest_filter=node.free_when_add
TEST(node, free_when_add) {
  auto getUsedNode = [&]() { return nodePool_.totalNodesCount() - nodePool_.freeNodesCount(); };
  auto usedNode = getUsedNode();
  NodeID root;
  auto make_it_busy = [&]() {
    auto w_root = nodePool_.GetWrapperNode();
    root = w_root->id_;
    auto w_child = nodePool_.GetWrapperNode();
    w_root->AddChildTraceNode(w_child);
    w_child->AddTraceDetail("E_ROOT_NODE", 234);
    std::this_thread::sleep_for(std::chrono::seconds(2));
  };
  std::thread t(make_it_busy);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  auto start = get_unix_time_ms();
  nodePool_.FreeNodeTree(root);
  nodePool_.FreeNodeTree(E_INVALID_NODE);
  nodePool_.FreeNodeTree(E_ROOT_NODE);
  auto end = get_unix_time_ms() - start;
  printf("it takes: %ld to free ", (long)end);
  t.join();
  EXPECT_EQ(usedNode, getUsedNode());
  EXPECT_TRUE(end >= 100);
}

//./bin/TestCommon --gtest_filter=node.orphan_node
TEST(node, orphan_node) {
  NodeID root, child_1, orphan;
  root = pinpoint_start_trace(E_ROOT_NODE);
  child_1 = pinpoint_start_trace(root);
  orphan = pinpoint_start_trace(child_1);
  pinpoint_end_trace(child_1);
  pinpoint_end_trace(root);

  root = pinpoint_start_trace(E_ROOT_NODE);
  child_1 = pinpoint_start_trace(E_ROOT_NODE);
  debug_nodeid(orphan);
  pinpoint_end_trace(orphan);
  debug_nodeid(orphan);
  pinpoint_end_trace(child_1);
  pinpoint_end_trace(root);
}
//./bin/TestCommon --gtest_filter=node.orphan_node_01
TEST(node, orphan_node_01) {
  pinpoint_set_agent("tcp:127.0.0.1:9999", 7000, -1, 7000);
  NodeID root, child_1, orphan;
  root = pinpoint_start_trace(E_ROOT_NODE);
  child_1 = pinpoint_start_trace(root);
  pinpoint_end_trace(root);
  orphan = pinpoint_start_trace(child_1);

  pinpoint_end_trace(child_1);
  pinpoint_end_trace(orphan);
}
//./bin/TestCommon --gtest_filter=node.orphan_root_parent_end
TEST(node, orphan_parent_root_end) {
  pinpoint_set_agent("tcp:127.0.0.1:9999", 7000, -1, 7000);
  NodeID root, child_1, orphan;
  root = pinpoint_start_trace(E_ROOT_NODE);
  child_1 = pinpoint_start_trace(root);
  orphan = pinpoint_start_trace(child_1);
  pinpoint_end_trace(child_1);
  pinpoint_end_trace(root);

  pinpoint_end_trace(orphan);
}

TEST(node, orphan_root_parent_end) {
  pinpoint_set_agent("tcp:127.0.0.1:9999", 7000, -1, 7000);
  NodeID root, child_1, orphan;
  root = pinpoint_start_trace(E_ROOT_NODE);
  child_1 = pinpoint_start_trace(root);
  orphan = pinpoint_start_trace(child_1);
  pinpoint_end_trace(root);
  pinpoint_end_trace(child_1);

  pinpoint_end_trace(orphan);
}
// ./bin/TestCommon --gtest_filter=node.end_trace_in_mt
TEST(node, end_trace_in_mt) {
  pinpoint_set_agent("tcp:127.0.0.1:9999", 7000, -1, 7000);
  NodeID root = pinpoint_start_trace(E_ROOT_NODE);
  NodeID next = root;
  // limit size 100; due to
  // https://github.com/pinpoint-apm/pinpoint-c-agent/runs/6806024797?check_suite_focus=true bus
  // error under macos
  for (int i = 0; i < 100; i++) {
    next = pinpoint_start_trace(next);
    pinpoint_end_trace(next);
  }

  std::mutex mtx;
  std::condition_variable cv;

  auto thread_func = [&]() {
    std::unique_lock<std::mutex> lck(mtx);
    cv.wait(lck);
    pp_trace("%lu ", std::this_thread::get_id());
    pinpoint_end_trace(root);
  };
  std::vector<std::thread> threads;
  for (int i = 0; i < 10; i++) {
    // std::thread t(thread_func);
    threads.emplace_back(thread_func);
  }
  // wait for all threads running
  sleep(2);
  cv.notify_all();
  for (auto& thread : threads)
    thread.join();
}
// ./bin/TestCommon --gtest_filter=node.max_sub_nodes
TEST(node, max_sub_nodes) {
  pinpoint_set_agent("tcp:127.0.0.1:9999", 7000, -1, 7000);
  NodeID root = pinpoint_start_trace(E_ROOT_NODE);
  while (true) {
    NodeID next = pinpoint_start_trace(root);
    if (next == E_INVALID_NODE) {
      break;
    }
    pinpoint_end_trace(next);
  }
  pinpoint_end_trace(root);
}