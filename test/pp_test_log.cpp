////////////////////////////////////////////////////////////////////////////////
// Copyright 2018 NAVER Corp.
// 
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License.  You may obtain a copy
// of the License at
// 
//   http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
// License for the specific language governing permissions and limitations under
// the License.
////////////////////////////////////////////////////////////////////////////////
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "log.h"
#include "utility.h"

#include <unistd.h>
#include <stdint.h>
#include <sys/eventfd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define MAXLEN 1024

#if 0

TEST(log,init_free)
{
    set_system_log_level(PP_LOG_DEBUG);
    EXPECT_EQ(set_logfile_root_path(NULL),-1);
    EXPECT_EQ(set_logfile_root_path("/home/log"),-1);
    EXPECT_EQ(set_logfile_root_path("/home/log"),-1);
    char currentPath[MAXLEN];
    EXPECT_TRUE(getcwd(currentPath, MAXLEN));
    EXPECT_EQ(set_logfile_root_path(currentPath),0);
    log_message("hello",5);
    free_log();
    unlink(strcat(currentPath,"/pinpoint_log.txt"));
}

TEST(log,fork_and_log10W_times)
{
#define CHILDCOUNT 10
#define MSGCOUNT 100000
    // init eventfd

    /*
     If  EFD_SEMAPHORE  was  specified  and  the eventfd counter has a nonzero value, then a
read(2) returns 8  bytes containing the value 1, and the counter’s value is decremented by 1.
    */
    int efd =  eventfd(0,EFD_SEMAPHORE);
    if(efd == -1)
    {
        perror(" eventfd failed ");
        return ;
    }
    uint64_t u = 10;
    char currentPath[MAXLEN];
    EXPECT_TRUE(getcwd(currentPath, MAXLEN));
    EXPECT_EQ(set_logfile_root_path(currentPath),0);
    // fork 10 process
    int i = CHILDCOUNT;
    while(i--)
    {
        pid_t tpid= fork();
        if(tpid == 0) // child
        {
	        //3 wait eventfd
            read(efd,&u,sizeof(u));
    		int count = 100000;
            printf("%d start...",getpid());
    		while(count--)
    		{
    			log_message("01234567890\n",12);
    		}
    		exit(1);
        }
    }
    write(efd,&u,sizeof(u));
    i = CHILDCOUNT;
    while(i--)
    {
        int status;
        waitpid(-1,&status,0);
    }
	// check file
    strcat(currentPath,"/pinpoint_log.txt");
    FILE *f = fopen(currentPath, "r");
    char buf[512]={0};
    if( f == NULL)
    {
        printf("fopen %s pFIle fail with %s ",currentPath,strerror(errno));
        return ;
    }

    i = 0;
    while (fgets(buf, sizeof( buf), f))
    {
       i++;
       EXPECT_EQ(memcmp(buf,"01234567890\n",12),0);
    }
    fclose(f);
    EXPECT_EQ(i,CHILDCOUNT*MSGCOUNT);
    unlink(currentPath);
}

TEST(boost,thread_specific)
{
//    boost::thread_specific_ptr<char*> g_tsdLogKey;
//    boost::thread_specific_ptr<char*> g_tsdFullLogKey;
//    char ** p = g_tsdFullLogKey.get();
//    EXPECT_EQ(p ==  NULL);


}

#endif
