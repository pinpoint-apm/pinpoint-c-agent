#include <gtest/gtest.h>
#include "Cache/SharedObj.h"

#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>

using namespace testing;
using namespace std;



#ifdef __linux__

TEST(shm, create_and_use_the_shared_mem)
{
//create the tmpfile
    shm_unlink("pinpoint-php.shm");
    if(!checking_and_init())
    {
        EXPECT_TRUE(0)<<"failed to init shared_obj";
    }

    char* address = (char*)fetch_shared_obj_addr();

    strcpy(address,"hello pinpoint");
    
// fork a child to check result
    pid_t pid =fork();
    if(pid == 0)
    {
        if(!checking_and_init())
        {
               EXPECT_TRUE(0)<<"failed to init shared_obj";
        }
        char* address = (char*)fetch_shared_obj_addr();
        EXPECT_STREQ(address,"hello pinpoint");
        detach_shared_obj();
        exit(0);
    }
    
    waitpid(pid,NULL,0);
    detach_shared_obj();
    shm_unlink("pinpoint-php.shm");
}

#endif
