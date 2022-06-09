#include "common.h"
#include <gtest/gtest.h>
#include "Util/Helper.h"
#include "header.h"
#include "NodePool/PoolManager.h"
#include <condition_variable>
#include <thread>
#include <chrono>

using NodePool::PoolManager;
using NodePool::TraceNode;
using NodePool::WrapperTraceNode;
using namespace testing;
using NodePool::freeNodeTree;
namespace Json = AliasJson;

std::mutex cv_m;
std::condition_variable cv;
NodeID rootId = E_ROOT_NODE;

int usedNode()
{
    return PoolManager::getInstance().totoalNodesCount() - PoolManager::getInstance().freeNodesCount();
}

// note: as it known, there may leak some node
void func()
{
    std::unique_lock<std::mutex> lk(cv_m);
    cv.wait(lk);
    pinpoint_add_clues(rootId, "xxxx", "bbbbbbss", E_LOC_CURRENT);
    pinpoint_add_clue(rootId, "xxx", "bbbbbb", E_LOC_CURRENT);
    for (int i = 0; i < 100; ++i)
    {
        rootId = pinpoint_start_trace(rootId);
        pinpoint_set_context_key(rootId, "xxxx", "bbbbbb");
        std::this_thread::yield();
        char buf[1024];
        pinpoint_get_context_key(rootId, "xxxx", buf, 1024);
        std::cout << "read value:" << buf << " ";
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

TEST(node, multipleThread)
{
    // no crash, works fine
    NodeID root = pinpoint_start_trace(E_ROOT_NODE);

    std::vector<std::thread> threads;

    for (int i = 0; i < 10; i++)
    {
        threads.push_back(std::thread(func));
    }

    sleep(2);
    cv.notify_all();

    for (int i = 0; i < 10; i++)
    {
        threads[i].join();
    }
    pinpoint_end_trace(root);
    pinpoint_end_trace(root);
    // EXPECT_TRUE(PoolManager::getInstance().NoNodeLeak());
}

TEST(node, wakeTrace)
{
    NodeID root = pinpoint_start_trace(E_ROOT_NODE);
    NodeID child1 = pinpoint_start_trace(root);

    pinpoint_end_trace(child1);

    pinpoint_wake_trace(child1);
    pinpoint_wake_trace(root);
    pinpoint_wake_trace(NodeID(159));
    pinpoint_wake_trace(NodeID(1027));
    // do something
    sleep(1);
    pinpoint_end_trace(child1);

    pinpoint_end_trace(root);
    // EXPECT_TRUE(PoolManager::getInstance().NoNodeLeak());
}

void test_opt(TraceNode &node, const char *opt, ...)
{
    va_list args;
    va_start(args, opt);
    node.setOpt(opt, &args);
    va_end(args);
}

TEST(node, opt)
{
    TraceNode &node = PoolManager::getInstance().Take();

    test_opt(node, "TraceMinTimeMs:23", "TraceOnlyException", nullptr);

    node.cumulative_time = 22;
    EXPECT_FALSE(node.checkOpt());
    node.cumulative_time = 23;
    EXPECT_TRUE(node.checkOpt());

    node.cumulative_time = 0;
    node.mHasExp = true;
    EXPECT_TRUE(node.checkOpt());
    node.mHasExp = false;
    EXPECT_FALSE(node.checkOpt());

    PoolManager::getInstance().Restore(node);
    // EXPECT_TRUE(PoolManager::getInstance().NoNodeLeak());
}

static std::string span;
void capture(const char *msg)
{
    span = msg;
}
//./bin/TestCommon --gtest_filter=node.pinpoint_start_traceV1
TEST(node, pinpoint_start_traceV1)
{
    auto count = usedNode();
    register_span_handler(capture);
    NodeID root, child1;
    root = pinpoint_start_trace(E_ROOT_NODE);
    child1 = pinpoint_start_traceV1(root, "TraceMinTimeMs:23", nullptr);
    pinpoint_add_clue(child1, "name", "Take1sec", E_LOC_CURRENT);
    sleep(1);
    pinpoint_end_trace(child1);

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
    std::cout << span << std::endl;
    EXPECT_TRUE(span.find("Take1sec") != span.npos);
    EXPECT_TRUE(span.find("Exception") != span.npos);
    EXPECT_TRUE(span.find("TraceMinTimeMs:2000") == span.npos);
    EXPECT_TRUE(span.find("NoException") == span.npos);

    EXPECT_TRUE(span.find("childFromTraceMinTimeMs:23-3") == span.npos);
    EXPECT_TRUE(span.find("childFromTraceMinTimeMs:23-2") == span.npos);
    EXPECT_EQ(count, usedNode());
}

TEST(node, leak_node)
{
    auto count = PoolManager::getInstance().freeNodesCount();
    NodeID root, child1, child2;
    root = pinpoint_start_trace(E_ROOT_NODE);
    child1 = pinpoint_start_trace(root);
    child2 = pinpoint_start_trace(child1);
    // root goes first
    pinpoint_end_trace(root);

    pinpoint_end_trace(child1);
    pinpoint_end_trace(child2);

    EXPECT_EQ(count, PoolManager::getInstance().freeNodesCount());
    show_status();
}

TEST(node, tons_of_nodes_01)
{
    auto count = usedNode(); // PoolManager::getInstance().totoalNodesCount() - PoolManager::getInstance().freeNodesCount();
    NodeID root = pinpoint_start_trace(E_ROOT_NODE);
    for (int i = 0; i < 1000; i++)
    {
        NodeID child1 = pinpoint_start_trace(root);
        pinpoint_end_trace(child1);
    }
    pinpoint_end_trace(root);

    EXPECT_EQ(count, usedNode()); //);

    count = usedNode();
    root = pinpoint_start_trace(E_ROOT_NODE);
    NodeID child1 = root;
    for (int i = 0; i < 1000; i++)
    {
        NodeID child = pinpoint_start_trace(child1);
        mark_current_trace_status(root, E_OFFLINE);
        pinpoint_end_trace(child);
        child1 = child;
    }
    mark_current_trace_status(root, E_TRACE_BLOCK);
    pinpoint_end_trace(root);

    EXPECT_EQ(count, usedNode());
}

TEST(node, tons_of_nodes_2k)
{
    auto count = usedNode(); // PoolManager::getInstance().totoalNodesCount() - PoolManager::getInstance().freeNodesCount();
    NodeID root = pinpoint_start_trace(E_ROOT_NODE);
    NodeID next = root;
    for (int i = 0; i < 2000; i++)
    {
        NodeID child = pinpoint_start_trace(next);
        pinpoint_end_trace(child);
        if (i % 2 == 0)
        {
            next = child;
        }
    }
    pinpoint_end_trace(root);

    EXPECT_EQ(count, usedNode());
}

TEST(node, tons_of_nodes_leak)
{
    auto count = usedNode();

    NodeID root, child_1, child_2;
    root = pinpoint_start_trace(E_ROOT_NODE);

    child_1 = pinpoint_start_trace(root);
    child_2 = pinpoint_start_trace(child_1);

    pinpoint_end_trace(child_2);

    pinpoint_end_trace(root);
    pinpoint_end_trace(child_1);
    EXPECT_EQ(count, usedNode());
}

TEST(node, tons_of_nodes_free_all)
{
    auto count = usedNode();

    NodeID root, child_1, child_2;
    root = pinpoint_start_trace(E_ROOT_NODE);

    child_1 = pinpoint_start_trace(root);
    child_2 = pinpoint_start_trace(child_1);
    freeNodeTree(root);
    pinpoint_end_trace(child_2);

    pinpoint_end_trace(root);
    pinpoint_end_trace(child_1);
    EXPECT_EQ(count, usedNode());
}
//./bin/TestCommon --gtest_filter=node.free_when_add
TEST(node, free_when_add)
{
    auto count = usedNode();
    NodeID root;
    auto make_it_busy = [&]()
    {
        auto w_root = PoolManager::getInstance().GetWrapperNode();
        root = w_root->mPoolIndex;
        auto w_child = PoolManager::getInstance().GetWrapperNode();
        w_root->addChild(w_child);
        w_child->AddTraceDetail("E_ROOT_NODE", 234);
        std::this_thread::sleep_for(std::chrono::seconds(2));
    };
    std::thread t(make_it_busy);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    auto start = Helper::get_current_msec_stamp();
    freeNodeTree(root);
    freeNodeTree(E_INVALID_NODE);
    freeNodeTree(E_ROOT_NODE);
    auto end = Helper::get_current_msec_stamp() - start;
    printf("it takes: %ld to free ", end);
    t.join();
    EXPECT_EQ(count, usedNode());
    EXPECT_TRUE(end >= 100);
}

//./bin/TestCommon --gtest_filter=node.orphan_node
TEST(node, orphan_node)
{
    auto count = usedNode();
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
    EXPECT_EQ(count, usedNode());
}
//./bin/TestCommon --gtest_filter=node.orphan_node_01
TEST(node, orphan_node_01)
{
    auto count = usedNode();
    NodeID root, child_1, orphan;
    root = pinpoint_start_trace(E_ROOT_NODE);
    child_1 = pinpoint_start_trace(root);
    pinpoint_end_trace(root);
    orphan = pinpoint_start_trace(child_1);

    pinpoint_end_trace(child_1);
    pinpoint_end_trace(orphan);

    EXPECT_EQ(count, usedNode());
}
//./bin/TestCommon --gtest_filter=node.orphan_root_parent_end
TEST(node, orphan_parent_root_end)
{
    auto count = usedNode();
    NodeID root, child_1, orphan;
    root = pinpoint_start_trace(E_ROOT_NODE);
    child_1 = pinpoint_start_trace(root);
    orphan = pinpoint_start_trace(child_1);
    pinpoint_end_trace(child_1);
    pinpoint_end_trace(root);

    pinpoint_end_trace(orphan);

    EXPECT_EQ(count, usedNode());
}

TEST(node, orphan_root_parent_end)
{
    auto count = usedNode();
    NodeID root, child_1, orphan;
    root = pinpoint_start_trace(E_ROOT_NODE);
    child_1 = pinpoint_start_trace(root);
    orphan = pinpoint_start_trace(child_1);
    pinpoint_end_trace(root);
    pinpoint_end_trace(child_1);

    pinpoint_end_trace(orphan);

    EXPECT_EQ(count, usedNode());
}
// ./bin/TestCommon --gtest_filter=node.end_trace_in_mt
TEST(node, end_trace_in_mt)
{
    auto count = usedNode();
    NodeID root = pinpoint_start_trace(E_ROOT_NODE);
    NodeID next = root;
    // limit size 100; due to https://github.com/pinpoint-apm/pinpoint-c-agent/runs/6806024797?check_suite_focus=true bus error under macos
    for (int i = 0; i < 100; i++)
    {
        next = pinpoint_start_trace(next);
        pinpoint_end_trace(next);
    }

    std::mutex mtx;
    std::condition_variable cv;

    auto thread_func = [&]()
    {
        std::unique_lock<std::mutex> lck(mtx);
        cv.wait(lck);
        pp_trace("%lu ", std::this_thread::get_id());
        pinpoint_end_trace(root);
    };
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++)
    {
        // std::thread t(thread_func);
        threads.emplace_back(thread_func);
    }
    // wait for all threads running
    sleep(2);
    cv.notify_all();
    for (auto &thread : threads)
        thread.join();
    EXPECT_EQ(count, usedNode());
}