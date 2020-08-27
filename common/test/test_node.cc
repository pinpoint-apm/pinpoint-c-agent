#include "common.h"
#include <gtest/gtest.h>
#include "NodePool/PoolManager.h"

using NodePool::NodeID;
using NodePool::TraceNode;
using NodePool::PoolManager;
using namespace testing;

NodePool::PoolManager g_pool;

inline TraceNode& getParent(NodeID _id)
{
   if(_id == 0){
        return  g_pool.getFreeNode();
    }else{
        return  g_pool.refNodeById(_id);
    }
}

NodeID start_trace(NodeID _id)
{
    if(_id == 0){
        TraceNode&  node =  g_pool.getFreeNode();
        node["name"] = std::to_string(node.getId());
        return node.getId();
    } 

    TraceNode&  parent = g_pool.refNodeById(_id);
    TraceNode&  child  = g_pool.getFreeNode();
    parent.addChild(child);
    child["name"] = std::to_string(child.getId());
    return child.getId();
}

NodeID end_trace(NodeID _id)
{
    TraceNode& current = g_pool.refNodeById(_id);
    if(current.parent) {
        return current.parent->getId();
    }
    else{
        return current.getId();
    }    
}

NodeID currentId;

void print_tree(TraceNode &node,int indent)
{
    printf("[%u@%p]",node.getId(),node);
    if(node.next){
        printf("->");
        indent += 15;
        print_tree(*node.next,indent);
    }

    if(node.children){
        printf("\n  %*s|\n",indent," ");
        printf("  %*sv\n",indent," ");
        printf("%*s",indent," ");
        print_tree(*node.children,indent);
    }
}




Json::Value& collect_tree_info(TraceNode &node)
{

    Json::Value& value = node.getValue();

    if(! node.isLeaf())
    {
        TraceNode * pstart = node.children;
        while (pstart)
        {
            /* code */
            value["calls"].append(collect_tree_info(*pstart));
            pstart = pstart->next;
        }

    }

    return value;   
}


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


        // currentId = start_trace(currentId);
        //     currentId = start_trace(currentId);
        //     currentId = end_trace(currentId);

        //     currentId = start_trace(currentId);
        //         currentId = start_trace(currentId);
        //         currentId = end_trace(currentId);
        //         currentId = start_trace(currentId);
        //         currentId = end_trace(currentId);
        //         currentId = start_trace(currentId);
        //             currentId = start_trace(currentId);
        //                 currentId = start_trace(currentId);
        //                     currentId = start_trace(currentId);
        //                     currentId = end_trace(currentId);
        //                 currentId = end_trace(currentId);
        //             currentId = end_trace(currentId);
        //         currentId = end_trace(currentId);
        //         currentId = start_trace(currentId);
        //             currentId = start_trace(currentId);
        //                 currentId = start_trace(currentId);
        //                 currentId = end_trace(currentId);
        //             currentId = end_trace(currentId);
        //         currentId = end_trace(currentId);
        //     currentId = end_trace(currentId);
        //     currentId = start_trace(currentId);
        //     currentId = end_trace(currentId);
        // currentId = end_trace(currentId);

    

    TraceNode& current = g_pool.refNodeById(currentId);

    // print_tree(current,0);

    Json::FastWriter writer;
    writer.dropNullPlaceholders();
    writer.omitEndingLineFeed();
    Json::Value& oRoot =  collect_tree_info(current);

    std::string stdBody  = writer.write(oRoot);

    const char* span = "{\"calls\":[{\"calls\":[{\"calls\":[{\"calls\":[{\"calls\":[{\"name\":\"25\"}],\"name\":\"24\"}],\"name\":\"23\"},{\"calls\":[{\"calls\":[{\"calls\":[{\"name\":\"22\"}],\"name\":\"21\"}],\"name\":\"20\"}],\"name\":\"19\"},{\"name\":\"18\"},{\"name\":\"17\"}],\"name\":\"16\"},{\"name\":\"15\"}],\"name\":\"14\"},{\"calls\":[{\"calls\":[{\"calls\":[{\"calls\":[{\"name\":\"13\"}],\"name\":\"12\"}],\"name\":\"11\"},{\"calls\":[{\"calls\":[{\"calls\":[{\"name\":\"10\"}],\"name\":\"9\"}],\"name\":\"8\"}],\"name\":\"7\"},{\"name\":\"6\"},{\"name\":\"5\"}],\"name\":\"4\"},{\"name\":\"3\"}],\"name\":\"2\"}],\"name\":\"1\"}";
    EXPECT_STREQ(span,stdBody.c_str());
}

