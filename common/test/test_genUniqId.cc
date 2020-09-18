#include <gtest/gtest.h>
#include "common.h"

#include <sys/types.h>
#include <sys/wait.h>

using namespace testing;


TEST(common,unique_id)
{
    static const int TIMES=10000000;
    int64_t last=-1;
    // reset the unique id
    reset_unique_id();
    pid_t pid =  fork();
    if(pid == 0){ // child
        for(int i=0;i<TIMES;i++)
        {
            int64_t uid = generate_unique_id();
            ASSERT_NE(uid,last);
            last = uid;
        }
        exit(0);
    }else{
        for(int i=0;i<TIMES;i++)
        {
            int64_t uid = generate_unique_id();
            ASSERT_NE(uid,last);
            last = uid;
        }
    }

    int state;
    int w = waitpid(pid,&state, WUNTRACED | WCONTINUED);
    if (w == -1) {
        perror("waitpid");
        exit(EXIT_FAILURE);
    }
    int64_t id = generate_unique_id();
    EXPECT_EQ(id, TIMES*2);
    // reset the shm
}
