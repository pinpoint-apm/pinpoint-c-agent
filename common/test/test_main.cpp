#include <gtest/gtest.h>
#include "common.h"
PPAgentT global_agent_info = {
       "unix:./pinpoint_test.sock",
        1,
        10,
        1500,
        1
};

int main(int argc, char **argv) {
    register_error_cb(NULL);
    ::testing::InitGoogleTest(&argc, argv); 
    return RUN_ALL_TESTS();
}