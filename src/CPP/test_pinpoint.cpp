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
#include<iostream>
#include<string>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>


thread_local int32_t id = 0;
const char* app_id ="cpp_test_app";
const char* app_name ="cpp_test_name";
PPAgentT global_agent_info ={
    "tcp:dev-collector:9999",1000,-1,1300,1,nullptr,nullptr,nullptr
};

std::string get_sid()
{
    return std::to_string(rand()%100000000l);
}

std::string get_tid()
{
    std::string sid="";
    return sid + app_id +"^"+ std::to_string(pinpoint_start_time())+"^"+std::to_string(generate_unique_id());
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
    pinpoint_add_clue(id,PP_NEXT_SPAN_ID,get_sid().c_str(),E_CURRENT_LOC);
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
    pinpoint_add_clue(id,PP_APP_NAME, "cpp_app",E_CURRENT_LOC);
    pinpoint_add_clue(id,PP_APP_ID,"CPP_APP",E_CURRENT_LOC);

    random_sleep();

    test_func();
    test_mysql();
    test_httpclient();

    pinpoint_add_clue(id,PP_TRANSCATION_ID,get_tid().c_str(),E_CURRENT_LOC);
    pinpoint_add_clue(id,PP_SPAN_ID,get_sid().c_str(),E_CURRENT_LOC);
    pinpoint_add_clues(id,PP_HTTP_STATUS_CODE, "200",E_CURRENT_LOC);
    id = pinpoint_end_trace(id);
}

int main(int argc, char const *argv[])
{
    srand(time(nullptr));
    int i=0;
    for(;i<3;i++)
    {
        test_req();
        sleep(1);
    }
    return 0;
}
