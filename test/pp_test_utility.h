/*
 * pp_test_utility.h
 *
 *  Created on: Mar 20, 2017
 *      Author: bluse
 */

#ifndef TEST_PP_TEST_UTILITY_H_
#define TEST_PP_TEST_UTILITY_H_
#include "pinpoint_api.h"

#include <assert.h>
#include <unistd.h>
#include <stdlib.h>

typedef union _TFlag
{
    int Store;
    struct
    {
        int RmoveMem : 1;
        int Seg : 1;
    } Flag;
} TFlag;

extern "C"  void __gcov_flush(void);


static void fork_and_execute(void (*process_main)(TFlag), TFlag PSFlag)
{
    pid_t pid = fork();
    if (pid > 0) {
        return;
    } else if (pid == 0) {
        (*process_main)(PSFlag);
        exit(1);
    } else {
        LOGI(" process %d exit", pid);
        assert(0);
    }
}

#define WAIT_CHILDREN()\
    while (errno != ECHILD) \
    {\
        int status = 0;\
        pid_t pid = wait(&status);\
        LOGI(" process %d exit" ,pid);\
    } \
    errno = 0;


#define MAKE_SEG_ERROR() \
    do{\
        __gcov_flush();\
    *(int*) 0 = 10;\
    }while(0)


#endif /* TEST_PP_TEST_UTILITY_H_ */
