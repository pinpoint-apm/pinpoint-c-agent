#include <gtest/gtest.h>
#include <cstdlib>
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h> 
#include "common.h"
PPAgentT global_agent_info = {
       "unix:./pinpoint_test.sock",
        1,
        10,
        1500,
        1
};

void remove_shm_file()
{
    shm_unlink("pinpoint-php.shm");
}

int main(int argc, char **argv) {
    std::atexit(remove_shm_file);
    register_error_cb(NULL);
    ::testing::InitGoogleTest(&argc, argv); 
    return RUN_ALL_TESTS();
}