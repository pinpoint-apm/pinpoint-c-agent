#include <gtest/gtest.h>
#include <locale.h>
#include <regex.h>
#include <thread>
#include <chrono>
#include "common.h"

using namespace testing;

std::string ouputMsg;
void cc_log_error_cb(char *msg)
{
    ouputMsg += msg;
}

TEST(common, uid_all_in_one)
{
    // register_error_cb(cc_log_error_cb);
    // test_trace();
    int64_t startId = generate_unique_id();
    generate_unique_id();
    generate_unique_id();
    generate_unique_id();
    EXPECT_EQ(generate_unique_id(), 4 + startId);

#define _100K 100000

    pid_t pid = fork();
    if (pid == 0)
    {
        for (int i = 0; i < _100K; i++)
            generate_unique_id();
        exit(0);
    }

    for (int i = 0; i < _100K; i++)
        generate_unique_id();
    waitpid(pid, NULL, 0);
    //[0~9999]
    EXPECT_EQ(generate_unique_id(), (startId + 2 * _100K + 5));
}

TEST(common, start_end_trace)
{
    NodeID id = pinpoint_start_trace(E_ROOT_NODE);
    mark_current_trace_status(id, E_OFFLINE);
    EXPECT_EQ(pinpoint_trace_is_root(id), 1);
    id = pinpoint_start_trace(id);
    EXPECT_EQ(pinpoint_trace_is_root(id), 0);
    mark_current_trace_status(id, E_OFFLINE);
    EXPECT_EQ(pinpoint_trace_is_root(NodeID(-1023)), -1);
    EXPECT_EQ(pinpoint_trace_is_root(NodeID(1023)), -1);
    id = pinpoint_start_trace(id);

    mark_current_trace_status(id, E_TRACE_PASS);
    catch_error(id, "sdfasfas", "fsafdsfasd", 234);
    id = pinpoint_end_trace(id);

    id = pinpoint_end_trace(id);

    id = pinpoint_end_trace(id);
    EXPECT_EQ(id, 0);
}

TEST(common, context_check)
{
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

    pinpoint_set_context_long(id, "1024", 1024);
    pinpoint_set_context_long(NodeID(0), "1024", 1024);
    pinpoint_set_context_long(NodeID(1205), "1024", 1024);
    pinpoint_set_context_key(id, "adfadf23", "fadfaffadf");
    long value;
    EXPECT_EQ(pinpoint_get_context_long(id, "1024", &value), 0);
    EXPECT_EQ(value, 1024);
    pinpoint_end_trace(id);
}

TEST(common, error_checking)
{
    NodeID id = pinpoint_start_trace(E_ROOT_NODE);
    EXPECT_EQ(pinpoint_trace_is_root(id), 1);
    id = pinpoint_end_trace(id);
    EXPECT_EQ(id, 0);

    id = pinpoint_start_trace(NodeID(1280));
    EXPECT_EQ(id, E_INVALID_NODE);
}

static void test_per_thread_id_odd()
{
    NodeID id = pinpoint_get_per_thread_id();
    EXPECT_EQ(id, 0);
    id = NodeID(1);
    for (int i = 1; i < 10000; i++)
    {
        int index = (int)id;
        index += 2;
        id = NodeID(index);
        pinpoint_update_per_thread_id(id);
        std::this_thread::sleep_for(std::chrono::microseconds(1));
        EXPECT_EQ(pinpoint_get_per_thread_id(), i * 2 + 1);
    }
}

static void test_per_thread_id_even()
{
    NodeID id = pinpoint_get_per_thread_id();
    EXPECT_EQ(id, 0);

    for (int i = 1; i < 10000; i++)
    {
        int index = (int)id;
        index += 2;
        id = (NodeID)index;

        pinpoint_update_per_thread_id(id);
        std::this_thread::sleep_for(std::chrono::microseconds(1));
        EXPECT_EQ(pinpoint_get_per_thread_id(), i * 2);
    }
}

TEST(common, per_threadid)
{
    std::thread f1(test_per_thread_id_odd);
    std::thread f2(test_per_thread_id_even);
    f1.join();
    f2.join();
}

TEST(common, force_end_trace)
{
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
}

TEST(common, version)
{
    EXPECT_STREQ(pinpoint_agent_version(), AGENT_VERSION);
}

//./bin/unittest --gtest_filter=common.id_1
TEST(common, id_1)
{
    NodeID id = pinpoint_start_trace(E_ROOT_NODE);
    for (int i = 0; i < 1280; i++)
    {
        id = pinpoint_start_trace(id);
    }
    pinpoint_set_context_long(id, "1024", 1024);
}
