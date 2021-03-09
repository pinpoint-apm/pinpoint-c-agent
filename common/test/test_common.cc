#include <gtest/gtest.h>
#include <locale.h>
#include <regex.h>
#include <thread>
#include <chrono>
#include "common.h"

using namespace testing;

std::string ouputMsg;
void cc_log_error_cb(char*msg)
{
    ouputMsg+=msg;
}

// TEST(common, trace)
// {
//     register_error_cb(cc_log_error_cb);
//     global_agent_info.inter_flag |= E_UTEST;
//     pinpoint_set_special_key("key1","1");
//     EXPECT_EQ(pinpoint_start_trace(),1);
//     pinpoint_set_special_key("key2","2");
//     const char* key = "k1";
//     const char* value = "v1";
//     pinpoint_add_clue(key,value);
//     pinpoint_add_clues(key,value);
//     pinpoint_add_clues(key,value);
//     pinpoint_set_special_key("key3","3");
//     EXPECT_EQ( pinpoint_start_trace(),2);
//     const char* value3 = pinpoint_get_special_key("key3");
//     EXPECT_STREQ(value3,"3");

//     key = "k2";
//     value = "v2";
//     pinpoint_add_clue(key,value);
//     pinpoint_add_clues(key,value);
//     pinpoint_add_clues(key,value);
//     EXPECT_EQ( pinpoint_end_trace(),1);
//     value3 = pinpoint_get_special_key("key3");
//     EXPECT_STREQ(value3,"3");

//     value3 = pinpoint_get_special_key("key2");
//     EXPECT_STREQ(value3,"2");
    
//     value3 = pinpoint_get_special_key("key1");
//     EXPECT_STREQ(value3,"1");

//     EXPECT_EQ( pinpoint_end_trace(),0);

//     value3 = pinpoint_get_special_key("key1");
//     EXPECT_STREQ(value3,NULL);

//     value3 = pinpoint_get_special_key("key2");
//     EXPECT_STREQ(value3,NULL);

//     value3 = pinpoint_get_special_key("key3");
//     EXPECT_STREQ(value3,NULL);


// }

TEST(common, uid_all_in_one)
{
    // register_error_cb(cc_log_error_cb);
    // test_trace();
    int64_t startId =  generate_unique_id();
    generate_unique_id();
    generate_unique_id();
    generate_unique_id();
    EXPECT_EQ(generate_unique_id(),4+ startId);

#define _100K 100000

    pid_t pid =fork();
    if(pid == 0){
        for(int i = 0;i<_100K;i++)
            generate_unique_id();
        exit(0);
    }

    for(int i = 0;i<_100K;i++)
        generate_unique_id();
    waitpid(pid,NULL,0);
    //[0~9999]
    EXPECT_EQ(generate_unique_id(),(startId + 2*_100K+5));
}


TEST(common,start_end_trace)
{
    NodeID id = pinpoint_start_trace(0);
    mark_current_trace_status(id,E_OFFLINE);
    EXPECT_EQ(pinpoint_trace_is_root(id),1);
    id = pinpoint_start_trace(id);
    EXPECT_EQ(pinpoint_trace_is_root(id),0);
    mark_current_trace_status(id,E_OFFLINE);
    EXPECT_EQ(pinpoint_trace_is_root(-1023),-1);
    EXPECT_EQ(pinpoint_trace_is_root(1023),-1);
    id = pinpoint_start_trace(id);

    mark_current_trace_status(id,E_TRACE_PASS);
    catch_error(id,"sdfasfas","fsafdsfasd",234);
    id = pinpoint_end_trace(id);

    id = pinpoint_end_trace(id);

    id = pinpoint_end_trace(id);
    EXPECT_EQ(id,0);
}


TEST(common,context_check)
{
    NodeID id = pinpoint_start_trace(0);
    std::string str="fdafadf";
    pinpoint_add_clues(id,"fasdfas",str.c_str(),E_CURRENT_LOC);
    pinpoint_add_clue(id,"fasdfas",str.c_str(),E_CURRENT_LOC);
    pinpoint_add_clue(id,"fasdfas",str.c_str(),E_CURRENT_LOC);
    pinpoint_add_clue(id,"fasdfas",str.c_str(),E_CURRENT_LOC);

    id = pinpoint_start_trace(id);
    pinpoint_add_clue(id,"global",str.c_str(),E_ROOT_LOC);
    id = pinpoint_end_trace(id);
    str.clear();
    str = "fadfaffadf";
    pinpoint_set_context_key(id,"adfadf",str.c_str());
    str+="35486we32";
    pinpoint_set_context_key(id,"adfadf23",str.c_str());
    str.clear();
    EXPECT_STREQ(pinpoint_get_context_key(id,"adfadf23"),"fadfaffadf35486we32");
    EXPECT_STREQ(pinpoint_get_context_key(id,"adfadf"),"fadfaffadf");

    pinpoint_set_context_long(id,"1024",1024);
    long value ;
    EXPECT_EQ(pinpoint_get_context_long(id,"1024",&value),0);
    EXPECT_EQ(value,1024);
    pinpoint_end_trace(id);
}

TEST(common,error_checking)
{
    NodeID id = pinpoint_start_trace(0);
    id = pinpoint_end_trace(id);
    EXPECT_EQ(id,0);
    id = pinpoint_start_trace(128);
    pinpoint_end_trace(128);
    EXPECT_EQ(id,0);
}


static void test_per_thread_id_odd()
{
    NodeID  id = pinpoint_get_per_thread_id();
    EXPECT_EQ(id,0);
    id = 1;
    for(int i =1;i<10000;i++)
    {
        id +=2;
        pinpoint_update_per_thread_id(id);
        std::this_thread::sleep_for(std::chrono::microseconds(1));
        EXPECT_EQ(pinpoint_get_per_thread_id(),i*2+1);
    }

}

static void test_per_thread_id_even()
{
    NodeID  id = pinpoint_get_per_thread_id();
    EXPECT_EQ(id,0);

    for(int i =1;i<10000;i++)
    {
        id +=2;
        pinpoint_update_per_thread_id(id);
        std::this_thread::sleep_for(std::chrono::microseconds(1));
        EXPECT_EQ(pinpoint_get_per_thread_id(),i*2);
    }
}

TEST(common,per_threadid)
{
    std::thread f1(test_per_thread_id_odd);
    std::thread f2(test_per_thread_id_even);
    f1.join();
    f2.join();
}

TEST(common,force_end_trace)
{
    NodeID id = pinpoint_start_trace(0);
    id = pinpoint_end_trace(id);
    id = pinpoint_start_trace(id);
    id = pinpoint_start_trace(id);
    id = pinpoint_start_trace(id);
    id = pinpoint_start_trace(id);
    id = pinpoint_end_trace(id);
    id = pinpoint_end_trace(id);
    EXPECT_NE(id,0);
    id = pinpoint_force_end_trace(id,300);
    EXPECT_EQ(id,0);
}


// int mymatch(char *buf)
// {
//     // const char *regex= "^([a-z0-9_\\.-]+)@([\\da-z\\.-]+)\\.([a-z\\.]{2,6})$";
//     const char *regex= "^([a-z0-9_\\.-]+)";
//     pcre *re; 
//     int ovector[3];
//     const size_t nmatch = 10;
//     const char *error;
//     int   erroffset;
//     regmatch_t pm[nmatch];
//     if (!(re =pcre_compile(regex,  PCRE_MULTILINE|PCRE_DOTALL|PCRE_NEWLINE_ANYCRLF, &error, &erroffset,  0) ) {  
//         exit(1);
//     }
//     int z, i;
//     z = pcre_exec(re,NULL, buf,strlen(buf),0,0,ovector,3);
//     if (z == REG_NOMATCH)
//     {    
//         return 0;
//     } 
//     else
//     { 
//         return nmatch;       
//         // for (i = 0; i < nmatch && pm[i].rm_so != -1; ++i)
//         // {
           
//         // }    
    
//     }
//     regfree(&preg);
// }
// #define OVECCOUNT 30

// int fun_ismatch( char* src, char* pattern)
// {
//         pcre *re;
//         const char *error;
//         int erroffset;
//         int rc;
//         int ovector[OVECCOUNT];

//         if( (re = pcre_compile( pattern, 0, &error, &erroffset, NULL)) == NULL) 
//         goto bad;
//         if( (rc = pcre_exec( re, NULL, src, strlen(src), 0, 0, ovector, OVECCOUNT)) < 0)
//          goto bad;

//         for (int i = 0; i < rc; i++) {
//                 char *substring_start = src + ovector[2*i];
//                 int substring_length = ovector[2*i+1] - ovector[2*i];
//                 printf("%2d: %.*s\n", i, substring_length, substring_start);
//         }

//         free(re);
//         return rc;
// bad:
//         free(re);
//         return -1;
// }

// TEST(test,reg)
// {
//     char name[]="liu@yeah.net";
//     char patter[]="^([a-z0-9_\\.-]+)@([\\da-z\\.-]+)\\.([a-z\\.]{2,6})jekkli$";
//     // char patter[]="^([a-z0-9_\\.-]+)@([\\da-z\\.-]+)\\.net$";
//     EXPECT_EQ(fun_ismatch(name,patter),1);
// }
