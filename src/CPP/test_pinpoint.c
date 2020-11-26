////////////////////////////////////////////////////////////////////////////////
// Copyright 2020 NAVER Corp
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

#include "common.h"
#include "pinpoint_define.h"

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>


int32_t id = 0;
const char* app_id ="c_test_app";
const char* app_name ="c_test_name";
PPAgentT global_agent_info ={
    "tcp:dev-collector:9999",1000,-1,1300,1,NULL,NULL,NULL
};

char* get_sid()
{
    char* psid = (char*)malloc(16);
    sprintf(psid,"%ld",rand()%100000000l);
    return psid;
}

char* get_tid()
{
    char* p_tid = (char*)malloc(64);
    sprintf(p_tid,"%s^%ld^%ld",app_id,pinpoint_start_time(),generate_unique_id());
    return p_tid;
}



void random_sleep()
{
    int32_t delay = rand()%10;
    usleep(delay* 1000);
}

void test_httpclient()
{
    id = pinpoint_start_trace(id);
    pinpoint_add_clue(id,PP_INTERCEPTOR_NAME,"httpclient",E_CURRENT_LOC);
    pinpoint_add_clue(id,PP_DESTINATION,"www.pinpoint-wonderful.com",E_CURRENT_LOC);
    pinpoint_add_clue(id,PP_SERVER_TYPE,PP_C_CPP_REMOTE_METHOD,E_CURRENT_LOC);
    char* sid = get_sid();
    pinpoint_add_clue(id,PP_NEXT_SPAN_ID,sid,E_CURRENT_LOC);
    free(sid);
    pinpoint_add_clues(id,PP_HTTP_URL,"/support/c-cpp-php-python",E_CURRENT_LOC);
    pinpoint_add_clue(id,PP_ADD_EXCEPTION,"test this exception",E_CURRENT_LOC);
    pinpoint_add_clues(id,PP_HTTP_STATUS_CODE,"300",E_CURRENT_LOC);

    random_sleep();

    id = pinpoint_end_trace(id);
}

void test_mysql()
{
    id = pinpoint_start_trace(id);
    pinpoint_add_clue(id,PP_INTERCEPTOR_NAME,"mysql::excute",E_CURRENT_LOC);
    pinpoint_add_clue(id,PP_SERVER_TYPE,PP_MYSQL,E_CURRENT_LOC);
    pinpoint_add_clue(id,PP_SQL_FORMAT,"select 1*3;",E_CURRENT_LOC);
    pinpoint_add_clue(id,PP_DESTINATION,"localhost:3307",E_CURRENT_LOC);
    random_sleep();
    id = pinpoint_end_trace(id);
}


void test_func()
{
    id = pinpoint_start_trace(id);
    pinpoint_add_clue(id,PP_INTERCEPTOR_NAME,"test_func",E_CURRENT_LOC);
    pinpoint_add_clue(id,PP_SERVER_TYPE,PP_C_CPP_METHOD,E_CURRENT_LOC);
    pinpoint_add_clues(id,PP_PHP_ARGS,"I'm the parameters",E_CURRENT_LOC);
    random_sleep();
    id = pinpoint_end_trace(id);
}


void test_req()
{
    id = pinpoint_start_trace(id);
    pinpoint_add_clue(id,PP_REQ_URI, "test_url",E_CURRENT_LOC);
    pinpoint_add_clue(id,PP_REQ_CLIENT, "127.0.0.1",E_CURRENT_LOC);
    pinpoint_add_clue(id,PP_REQ_SERVER, "HTTP_HOST",E_CURRENT_LOC);
    pinpoint_add_clue(id,PP_SERVER_TYPE, PP_C_CPP,E_CURRENT_LOC);
    pinpoint_add_clue(id,PP_INTERCEPTOR_NAME, "C_CPP Request",E_CURRENT_LOC);
    pinpoint_add_clue(id,PP_APP_NAME,app_name, E_CURRENT_LOC);
    pinpoint_add_clue(id,PP_APP_ID,app_id,E_CURRENT_LOC);

    random_sleep();

    test_func();
    test_mysql();
    test_httpclient();
    char* tid = get_tid();
    char* sid = get_sid();
    pinpoint_add_clue(id,PP_TRANSCATION_ID,tid,E_CURRENT_LOC);
    pinpoint_add_clue(id,PP_SPAN_ID,sid,E_CURRENT_LOC);
    pinpoint_add_clues(id,PP_HTTP_STATUS_CODE, "200",E_CURRENT_LOC);
    id = pinpoint_end_trace(id);
    free(tid);
    free(sid);
}

int main(int argc, char const *argv[])
{
    srand(time(NULL));
    int i=0;
    for(;i<3;i++)
    {
        test_req();
        sleep(1);
    }
    return 0;
}
