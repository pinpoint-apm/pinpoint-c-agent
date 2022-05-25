#include "common.h"
#include <gtest/gtest.h>
#include "Util/Helper.h"
#include "NodePool/PoolManager.h"
#include <condition_variable>
#include <thread>
#include <chrono>

using NodePool::PoolManager;
using NodePool::TraceNode;
using namespace testing;
namespace Json = AliasJson;

NodeID start_trace(NodeID _id)
{
    if (_id == 0)
    {
        TraceNode &node = PoolManager::getInstance().GetNode();
        node.setNodeValue("name", std::to_string(node.getId()).c_str());
        node.startTimer();
        return node.getId();
    }

    TraceNode &parent = PoolManager::getInstance().GetNode(_id);
    TraceNode &child = PoolManager::getInstance().GetNode();
    child.startTimer();
    child.setTraceParent(parent);
    child.setNodeValue("name", std::to_string(child.getId()).c_str());

    return child.ID;
}

NodeID end_trace(NodeID _id)
{
    TraceNode &node = PoolManager::getInstance().GetNode(_id);
    return node.mParentId == node.ID ? E_ROOT_NODE : node.mParentId;
}

// void free_nodes_tree(TraceNode &node)
// {
//     NodeID childId = node.mChildId;
//     if (childId != E_INVALID_NODE)
//     {
//         // keep the next child
//         TraceNode &child = PoolManager::getInstance().GetNode(childId);

//         childId = child.mNextId;
//         free_nodes_tree(child);
//     }
//     PoolManager::getInstance().freeNode(node);
// }

void print_tree(TraceNode &node, int indent)
{
    for (int i = 0; i < indent; i++)
        printf("|     ");

    printf("|---[%u]\n", node.getId());

    NodeID childId = node.mChildId;
    while (childId != E_INVALID_NODE)
    {
        TraceNode &child = PoolManager::getInstance().GetNode(node.mChildId);
        print_tree(child, indent + 1);
        childId = child.mNextId;
    }
}

// TEST(node, merge_children)
// {
//     NodeID currentId;
//     currentId = start_trace(E_ROOT_NODE); //# 128

//     currentId = start_trace(currentId); //# 127

//     currentId = end_trace(currentId); //# 128

//     currentId = start_trace(currentId); //# 126
//     std::cout << "currentId:" << currentId << std::endl;
//     currentId = end_trace(currentId); //# 128

//     std::cout << "currentId:" << currentId << std::endl;
//     currentId = end_trace(currentId);
//     std::cout << "currentId:" << currentId << std::endl;

//     TraceNode &rootNode = PoolManager::getInstance().getNodeById(currentId);
//     rootNode.endTimer();
//     // rootNode.convertToSpan();
//     Json::Value oRoot = Helper::mergeTraceNodeTree(rootNode);
//     std::string stdBody = Helper::node_tree_to_string(oRoot);
//     free_nodes_tree(rootNode);
//     EXPECT_STREQ(stdBody.c_str(), "{\"calls\":[{\"name\":\"127\"},{\"name\":\"126\"}],\"name\":\"128\"}");
// }

std::mutex cv_m;
std::condition_variable cv;
NodeID rootId = E_ROOT_NODE;

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
        const char *value = pinpoint_get_context_key(rootId, "xxxx");
        std::cout << "read value:" << value << " ";
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
    TraceNode &node = PoolManager::getInstance().GetNode();

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

    PoolManager::getInstance().freeNode(node);
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
    auto count = PoolManager::getInstance().freeNodesCount();
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
    pinpoint_end_trace(child1);

    child1 = pinpoint_start_traceV1(root, "TraceOnlyException", nullptr);
    pinpoint_add_clue(child1, "name", "NoException", E_LOC_CURRENT);
    pinpoint_end_trace(child1);

    child1 = pinpoint_start_traceV1(root, "TraceMinTimeMs:-23", nullptr);
    pinpoint_add_clue(child1, "name", "TraceMinTimeMs:-23", E_LOC_CURRENT);
    pinpoint_end_trace(child1);

    pinpoint_end_trace(root);
    std::cout << span << std::endl;
    EXPECT_EQ(count, PoolManager::getInstance().freeNodesCount());
    EXPECT_TRUE(span.find("Take1sec") != span.npos);
    EXPECT_TRUE(span.find("Exception") != span.npos);
    EXPECT_TRUE(span.find("TraceMinTimeMs:2000") == span.npos);
    EXPECT_TRUE(span.find("NoException") == span.npos);
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