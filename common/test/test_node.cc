#include "common.h"
#include <gtest/gtest.h>
#include "../src/Util/Helper.h"
#include "NodePool/PoolManager.h"
#include <condition_variable>
#include <thread>
#include <chrono>

using NodePool::TraceNode;
using NodePool::PoolManager;
using namespace testing;
namespace Json = AliasJson;

static NodePool::PoolManager g_pool;


inline TraceNode& getParent(NodeID _id)
{
   if(_id == 0){
        return  g_pool.getNode();
    }else{
        return  g_pool.getNodeById(_id);
    }
}

NodeID start_trace(NodeID _id)
{
    if(_id == 0){
        TraceNode&  node =  g_pool.getNode();
        node.setNodeValue("name", std::to_string(node.getId()).c_str());
        return node.getId();
    } 

    TraceNode&  parent = g_pool.getNodeById(_id);
    TraceNode&  child  = g_pool.getNode();
    parent.addChild(child);
    child.setNodeValue("name", std::to_string(child.getId()).c_str());
    return child.getId();
}

NodeID end_trace(NodeID _id)
{
    TraceNode& current = g_pool.getNodeById(_id);
    if(current.p_parent_node) {
        return current.p_parent_node->getId();
    }
    else{
        return current.getId();
    }    
}

void free_nodes_tree(TraceNode *node)
{
    if(node == nullptr) return ;

    TraceNode * p_child = node->p_child_head;
    while (p_child) // free all children
    {
        // keep the next child
        TraceNode * p_bro = p_child->p_brother_node;
        // free current child tree
        free_nodes_tree(p_child);
        // go on
        p_child = p_bro;
    }
    // free self
    g_pool.freeNode(*node);
    // pp_trace("give %d",node->getId());
}

NodeID currentId;


void print_tree(TraceNode*node,int indent)
{
    for(int i = 0;i<indent;i++)
        printf("|     ");

    printf("|---[%u@%p]\n",node->getId(),node);
    TraceNode* child = node->p_child_head;
    while(child){
        print_tree(child,indent+1);
        child = child->p_brother_node;
    }
}

Json::Value& collect_tree_info(TraceNode &node)
{

    Json::Value& value = node.getValue();

    if(! node.isLeaf())
    {
        TraceNode * pstart = node.p_child_head;
        while (pstart)
        {
            /* code */
            value["calls"].append(collect_tree_info(*pstart));
            pstart = pstart->p_brother_node;
        }

    }

    return value;   
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
TEST(node, node_tree)
{
    currentId = start_trace(0);

        currentId = start_trace(currentId);
            currentId = start_trace(currentId);
            currentId = end_trace(currentId);

            currentId = start_trace(currentId);
                currentId = start_trace(currentId);
                currentId = end_trace(currentId);

                currentId = start_trace(currentId);
                currentId = end_trace(currentId);

                    currentId = start_trace(currentId);
                        currentId = start_trace(currentId);
                            currentId = start_trace(currentId);
                                currentId = start_trace(currentId);
                                currentId = end_trace(currentId);
                            currentId = end_trace(currentId);
                        currentId = end_trace(currentId);
                        // pinpoint_add_clue(currentId,"global","test",E_ROOT_LOC);
                    currentId = end_trace(currentId);

                    currentId = start_trace(currentId);
                        currentId = start_trace(currentId);
                            currentId = start_trace(currentId);
                            currentId = end_trace(currentId);
                        currentId = end_trace(currentId);
                    currentId = end_trace(currentId);
                currentId = end_trace(currentId);
            currentId = end_trace(currentId);
        currentId = end_trace(currentId);

              currentId = start_trace(currentId);
            currentId = start_trace(currentId);
            currentId = end_trace(currentId);

            currentId = start_trace(currentId);
                currentId = start_trace(currentId);
                currentId = end_trace(currentId);

                currentId = start_trace(currentId);
                currentId = end_trace(currentId);

                    currentId = start_trace(currentId);
                        currentId = start_trace(currentId);
                            currentId = start_trace(currentId);
                                currentId = start_trace(currentId);
                                currentId = end_trace(currentId);
                            currentId = end_trace(currentId);
                        currentId = end_trace(currentId);
                    currentId = end_trace(currentId);

                    currentId = start_trace(currentId);
                        currentId = start_trace(currentId);
                            currentId = start_trace(currentId);
                            currentId = end_trace(currentId);
                        currentId = end_trace(currentId);
                    currentId = end_trace(currentId);
                currentId = end_trace(currentId);
            currentId = end_trace(currentId);
        currentId = end_trace(currentId);
    currentId = end_trace(currentId);


    TraceNode& current = g_pool.getNodeById(currentId);

    // print_tree(&current,0);

    Json::Value& oRoot =  Helper::merge_node_tree(current);
    // std::string stdBody  = writer.write(oRoot); //Trace::node_tree_to_string(oRoot);
    std::string stdBody = Helper::node_tree_to_string(oRoot);
    free_nodes_tree(&current);
    const char* span = "{\"calls\":[{\"calls\":[{\"name\":\"126\"},{\"calls\":[{\"name\":\"124\"},{\"name\":\"123\"},{\"calls\":[{\"calls\":[{\"calls\":[{\"name\":\"119\"}],\"name\":\"120\"}],\"name\":\"121\"}],\"name\":\"122\"},{\"calls\":[{\"calls\":[{\"name\":\"116\"}],\"name\":\"117\"}],\"name\":\"118\"}],\"name\":\"125\"}],\"name\":\"127\"},{\"calls\":[{\"name\":\"114\"},{\"calls\":[{\"name\":\"112\"},{\"name\":\"111\"},{\"calls\":[{\"calls\":[{\"calls\":[{\"name\":\"107\"}],\"name\":\"108\"}],\"name\":\"109\"}],\"name\":\"110\"},{\"calls\":[{\"calls\":[{\"name\":\"104\"}],\"name\":\"105\"}],\"name\":\"106\"}],\"name\":\"113\"}],\"name\":\"115\"}],\"name\":\"128\"}";
    EXPECT_STREQ(span,stdBody.c_str());
    EXPECT_EQ(g_pool.totoalNodesCount(),128);
    EXPECT_EQ(g_pool.freeNodesCount(),128);

}

#pragma GCC diagnostic pop


TEST(node,merge_children)
{
    currentId = start_trace(0);

        currentId = start_trace(currentId);
        
        currentId = end_trace(currentId);
        currentId = start_trace(currentId);
        currentId = end_trace(currentId);
    currentId = end_trace(currentId);

    TraceNode& current = g_pool.getNodeById(currentId);

    Json::Value& oRoot =  Helper::merge_node_tree(current);
    // std::string stdBody  = writer.write(oRoot); //Trace::node_tree_to_string(oRoot);
    std::string stdBody = Helper::node_tree_to_string(oRoot);
    free_nodes_tree(&current);
    std::cout<<stdBody;
    EXPECT_STREQ(stdBody.c_str(),"{\"calls\":[{\"name\":\"127\"},{\"name\":\"126\"}],\"name\":\"128\"}");

}

std::mutex cv_m; 
std::condition_variable cv;
NodeID func_Id;

void func(){
    std::unique_lock<std::mutex> lk(cv_m);
    cv.wait(lk);
    pinpoint_add_clues(func_Id,"xxxx","bbbbbbss",E_CURRENT_LOC);
    pinpoint_add_clue(func_Id,"xxx","bbbbbb",E_CURRENT_LOC);
    for (int i = 0; i < 10; ++i)
    {
        func_Id = pinpoint_start_trace(func_Id);
        pinpoint_set_context_key(func_Id,"xxxx","bbbbbb");
        std::this_thread::yield();
        const char*value = pinpoint_get_context_key(func_Id,"xxxx");
        std::cout<<"read value:"<<value<<" ";
        pinpoint_add_clues(func_Id,"xxxx","bbbbbbss",E_CURRENT_LOC);
        std::this_thread::yield();
        pinpoint_add_clue(func_Id,"xxx","bbbbbb",E_CURRENT_LOC);
        std::this_thread::yield();
        func_Id = pinpoint_end_trace(func_Id);
        std::this_thread::yield();
    }
    pinpoint_add_clues(func_Id,"xxxx","bbbbbbss",E_CURRENT_LOC);
    pinpoint_add_clue(func_Id,"xxx","bbbbbb",E_CURRENT_LOC);
}

TEST(node,multipleThread)
{
    // no crash, works fine
    NodeID func_Id = pinpoint_start_trace(0);
    
    std::vector<std::thread> threads;

    for(int i = 0; i < 10; i++){
        threads.push_back(std::thread(func));
    }

    sleep(2);
    cv.notify_all();

    for(int i = 0; i < 10; i++){
        threads[i].join();
    }
    pinpoint_end_trace(func_Id);
    pinpoint_end_trace(func_Id);
}