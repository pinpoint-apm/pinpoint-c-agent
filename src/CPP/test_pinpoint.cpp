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

}

void test_mysql()
{

}


void test_func()
{

}


void test_req()
{
    id = pinpoint_start_trace(id);
    pinpoint_add_clue(id,PP_REQ_URI, "test_url",E_CURRENT_LOC);
    pinpoint_add_clue(id,PP_REQ_CLIENT, "127.0.0.1",E_CURRENT_LOC);
    pinpoint_add_clue(id,PP_REQ_SERVER, "HTTP_HOST",E_CURRENT_LOC);
    pinpoint_add_clue(id,PP_SERVER_TYPE, PP_C_CPP,E_CURRENT_LOC);
    pinpoint_add_clue(id,PP_INTERCEPTER_NAME, "C_CPP Request",E_CURRENT_LOC);
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
