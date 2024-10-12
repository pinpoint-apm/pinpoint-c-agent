#include "pinpoint/common.h"
#include "json/value.h"
#include <gtest/gtest.h>
#include "header.h"
#define COMMON_DEBUG
#include "NodePool/PoolManager.h"
#include <condition_variable>
#include <thread>
#include <chrono>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iostream>

using PP::NodePool::PoolManager;
using PP::NodePool::ThreadSafePoolManager;
using PP::NodePool::TraceNode;
using PP::NodePool::WrapperTraceNodePtr;
using namespace testing;
namespace Json = AliasJson;

static void test_opt(TraceNode& node, const char* opt, ...) {
  va_list args;
  va_start(args, opt);
  node.setNodeUserOption(opt, &args);
  va_end(args);
}

TEST(NodePool, opt) {
  PoolManager nodePool_;
  TraceNode& node = nodePool_.GetNode();

  test_opt(node, "TraceMinTimeMs:23", "TraceOnlyException", nullptr);

  node.expired_time_ = 22;
  EXPECT_FALSE(node.runUserOptionFunc());
  node.expired_time_ = 23;
  EXPECT_TRUE(node.runUserOptionFunc());

  node.expired_time_ = 0;
  node.set_exp_ = true;
  EXPECT_TRUE(node.runUserOptionFunc());
  node.set_exp_ = false;
  EXPECT_FALSE(node.runUserOptionFunc());

  nodePool_.ReturnNode(node.id_);
  EXPECT_TRUE(nodePool_.NoNodeLeak());
}

TEST(NodePool, leak_node) {
  PoolManager nodePool_;
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

TEST(NodePool, tons_of_nodes_01) {
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
  for (int i = 0; i < 3000; i++) {
    NodeID child = pinpoint_start_trace(child1);
    change_trace_status(root, E_OFFLINE);
    pinpoint_end_trace(child);
    child1 = child;
  }
  change_trace_status(root, E_TRACE_BLOCK);
  pinpoint_end_trace(root);
}

TEST(NodePool, tons_of_nodes_1k) {
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

TEST(NodePool, tons_of_nodes_leak) {
  NodeID root, child_1, child_2;
  root = pinpoint_start_trace(E_ROOT_NODE);

  child_1 = pinpoint_start_trace(root);
  child_2 = pinpoint_start_trace(child_1);

  pinpoint_end_trace(child_2);

  pinpoint_end_trace(root);
  pinpoint_end_trace(child_1);
}

TEST(NodePool, tons_of_nodes_free_all) {

  NodeID root, child_1, child_2;
  root = pinpoint_start_trace(E_ROOT_NODE);

  child_1 = pinpoint_start_trace(root);
  child_2 = pinpoint_start_trace(child_1);
  pinpoint_end_trace(child_2);

  pinpoint_end_trace(root);
  pinpoint_end_trace(child_1);
}
//./bin/TestCommon --gtest_filter=NodePool.free_when_add
TEST(NodePool, free_when_add) {
  ThreadSafePoolManager nodePool_;
  auto getUsedNode = [&]() { return nodePool_.totalNodesCount() - nodePool_.freeNodesCount(); };
  auto usedNode = getUsedNode();
  NodeID root;
  auto make_it_busy = [&]() {
    auto& w_root = nodePool_.GetNode();
    root = w_root.id_;
    auto& w_child = nodePool_.GetNode();

    WrapperTraceNodePtr parent_ptr = w_root;
    w_child.BindParentTrace(parent_ptr);

    w_child.AddAnnotation("E_ROOT_NODE", 234);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    nodePool_.ReturnNode(w_child.id_);
  };
  std::thread t(make_it_busy);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  auto start = get_unix_time_ms();
  nodePool_.FreeNodeTree(root);
  nodePool_.FreeNodeTree(E_INVALID_NODE);
  nodePool_.FreeNodeTree(E_ROOT_NODE);
  auto end = get_unix_time_ms() - start;
  pp_trace("it takes: %ld to free ", (long)end);
  t.join();
  EXPECT_EQ(usedNode, getUsedNode());
  EXPECT_TRUE(end >= 100);
}

//./bin/TestCommon --gtest_filter=node.orphan_node
TEST(NodePool, orphan_node) {
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
TEST(NodePool, orphan_node_01) {
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
TEST(NodePool, orphan_parent_root_end) {
  pinpoint_set_agent("tcp:127.0.0.1:9999", 7000, -1, 7000);
  NodeID root, child_1, orphan;
  root = pinpoint_start_trace(E_ROOT_NODE);
  child_1 = pinpoint_start_trace(root);
  orphan = pinpoint_start_trace(child_1);
  pinpoint_end_trace(child_1);
  pinpoint_end_trace(root);

  pinpoint_end_trace(orphan);
}

TEST(NodePool, orphan_root_parent_end) {
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
TEST(NodePool, end_trace_in_mt) {
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

// ./bin/TestCommon --gtest_filter=NodePool.max_sub_nodes
TEST(NodePool, max_sub_nodes) {
  pinpoint_set_agent("tcp:127.0.0.1:9999", 7000, -1, 7000);
  NodeID root = pinpoint_start_trace(E_ROOT_NODE);
  while (true) {
    NodeID next = pinpoint_start_trace(root);
    if (next == E_INVALID_NODE) {
      pp_trace(" pinpoint_start_trace return E_INVALID_NODE ");
      break;
    }
    pinpoint_end_trace(next);
  }
  pinpoint_end_trace(root);
}

TEST(NodePool, get_and_give_back) {
  PoolManager pool;
  // new
  TraceNode& _node = pool.GetNode();
  void* p = &_node;
  NodeID id = _node.getId();
  NodeID next;
  // give back
  next = pool.ReturnNode(id);
  EXPECT_EQ(next, E_INVALID_NODE);
  TraceNode& _node_01 = pool.GetNode();

  TraceNode& new_node = pool.GetNode();
  NodeID new_id = new_node.id_;

  auto ref_node = pool.ReferNode(new_id);
  EXPECT_EQ(ref_node->id_, new_node.id_);

  next = pool.ReturnNode(new_id);
  EXPECT_THROW(pool.ReferNode(new_id), std::out_of_range);

  // ref current
  auto ref_new_node = pool.ReferNode(_node_01.getId());

  EXPECT_EQ(p, &_node);
  // reuse the same id
  EXPECT_EQ(id, _node.getId());
  EXPECT_THROW(pool.ReferNode(NodeID(100)), std::out_of_range);
  EXPECT_THROW(pool.ReferNode(NodeID(10000)), std::out_of_range);
}

static ThreadSafePoolManager thread_safe_pool;

void test_node_pool(bool& result) {
  NodeID it = E_INVALID_NODE;
  for (int i = 0; i < 1000; i++) {
    TraceNode& _node = thread_safe_pool.GetNode();
    usleep(1000);
    if (_node.getId() == it) {
      result = false;
      return;
    }
    thread_safe_pool.ReturnNode(_node.id_);
  }
  result = true;
}

//  ./bin/TestCommon --gtest_filter=NodePool.get_and_give_back_tls
TEST(NodePool, get_and_give_back_tls) {
  bool rth1, rth2;
  std::thread t1(test_node_pool, std::ref(rth1));
  std::thread t2(test_node_pool, std::ref(rth2));

  t1.join();
  t2.join();
  EXPECT_TRUE(rth1);
  EXPECT_TRUE(rth2);
}

//  ./bin/TestCommon --gtest_filter=NodePool.double_append
TEST(NodePool, double_append) {
  PoolManager pool;
  NodeID root_id;
  {
    auto& root = pool.GetNode();
    root.UpgradeToRootNode(1500);
    root_id = root.id_;
    auto& child_0 = pool.GetNode();

    child_0.BindParentTrace(root);
    PP::NodePool::WrapperTraceNodePtr root_ptr(root);
    pool.AppendToRootTrace(root_ptr, child_0);
    pool.AppendToRootTrace(root_ptr, child_0);
    pool.AppendToRootTrace(root_ptr, root);
  }
  pool.FreeNodeTree(root_id);
  EXPECT_TRUE(pool.NoNodesLeak());
}