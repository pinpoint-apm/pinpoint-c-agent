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
        TraceNode &node = PoolManager::getInstance().getNode();
        node.setNodeValue("name", std::to_string(node.getId()).c_str());
        node.startTimer();
        return node.getId();
    }

    TraceNode &parent = PoolManager::getInstance().getNodeById(_id);
    TraceNode &child = PoolManager::getInstance().getNode();
    child.startTimer();
    child.setTraceParent(parent);
    child.setNodeValue("name", std::to_string(child.getId()).c_str());

    return child.ID;
}

NodeID end_trace(NodeID _id)
{
    TraceNode &node = PoolManager::getInstance().getNodeById(_id);
    return node.mParentId == node.ID ? E_ROOT_NODE : node.mParentId;
}

void free_nodes_tree(TraceNode &node)
{
    NodeID childId = node.mChildId;
    if (childId != E_INVALID_NODE)
    {
        // keep the next child
        TraceNode &child = PoolManager::getInstance().getNodeById(childId);

        childId = child.mNextId;
        free_nodes_tree(child);
    }
    PoolManager::getInstance().freeNode(node);
}

void print_tree(TraceNode &node, int indent)
{
    for (int i = 0; i < indent; i++)
        printf("|     ");

    printf("|---[%u]\n", node.getId());

    NodeID childId = node.mChildId;
    while (childId != E_INVALID_NODE)
    {
        TraceNode &child = PoolManager::getInstance().getNodeById(node.mChildId);
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

void func()
{
    std::unique_lock<std::mutex> lk(cv_m);
    cv.wait(lk);
    pinpoint_add_clues(rootId, "xxxx", "bbbbbbss", E_CURRENT_LOC);
    pinpoint_add_clue(rootId, "xxx", "bbbbbb", E_CURRENT_LOC);
    for (int i = 0; i < 10; ++i)
    {
        rootId = pinpoint_start_trace(rootId);
        pinpoint_set_context_key(rootId, "xxxx", "bbbbbb");
        std::this_thread::yield();
        const char *value = pinpoint_get_context_key(rootId, "xxxx");
        std::cout << "read value:" << value << " ";
        pinpoint_add_clues(rootId, "xxxx", "bbbbbbss", E_CURRENT_LOC);
        std::this_thread::yield();
        pinpoint_add_clue(rootId, "xxx", "bbbbbb", E_CURRENT_LOC);
        std::this_thread::yield();
        rootId = pinpoint_end_trace(rootId);
        std::this_thread::yield();
    }
    pinpoint_add_clues(rootId, "xxxx", "bbbbbbss", E_CURRENT_LOC);
    pinpoint_add_clue(rootId, "xxx", "bbbbbb", E_CURRENT_LOC);
}

TEST(node, multipleThread)
{
    // no crash, works fine
    NodeID func_Id = pinpoint_start_trace(E_ROOT_NODE);

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
    pinpoint_end_trace(func_Id);
    pinpoint_end_trace(func_Id);
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
}
