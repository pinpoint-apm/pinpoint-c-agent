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
#include "pinpoint_api.h"
#include "server.h"
#include "server_handler.h"
#include "collector_context.h"
#include "simulate_def.h"
#include "checker_handler.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#ifndef TEST_SIMULATE

#endif

using namespace Pinpoint::log;

//const char* default_ip = "10.34.135.155";
//const char* configFileName = "../conf/pinpoint_collector.conf";
#define DEFAULT_LOGPATH "/tmp"
#define DEFAULT_LOGLEVER "DEBUG"
#define INVALID_PORT -1

static int32_t get_conf(const char* configPath);
static void usage();


int main(int argc, char *argv[])
{
    int32_t err = Pinpoint::SUCCESS;

    if (argc != 2)
    {
        usage();
        return -1;
    }

    err = get_conf(argv[1]);
    if (err != Pinpoint::SUCCESS)
    {
        LOGE("get_conf failed.");
        return -1;
    }

    CheckerPtrVec checkers;
    err = get_checkers(checkers);
    if (err != Pinpoint::SUCCESS)
    {
        LOGE("get_checkers failed.");
        return -1;
    }

    for (CheckerPtrVec::iterator ip = checkers.begin(); ip != checkers.end(); ++ip)
    {
        err = (*ip)->check();
        if (err != Pinpoint::SUCCESS)
        {
            LOGE("check failed.");
            return -1;
        }
    }

    return 0;
}


void usage()
{
    printf("Usage: test_checker configFilePath.\n");
}

int32_t get_conf(const char* configPath)
{
    CollectorContext& context = CollectorContext::getContext();

    context.setConfigPath(configPath);

    Pinpoint::Configuration::ConfFileObject fileobj(configPath);

    Pinpoint::Configuration::Config config(fileobj);

    Pinpoint::log::set_logging_file(config.readString( "common.LogFileRootPath",
                                                       DEFAULT_LOGPATH).c_str());
    Pinpoint::log::set_log_level(config.readString( "common.PPLogLevel",
                                                    DEFAULT_LOGLEVER).c_str());


    std::string testCasePath = config.readString("simulate.TestCasePath", "");
    if (testCasePath == "")
    {
        LOGE("invalid testCasePath.");
        return Pinpoint::FAILED;
    }
    context.setTestCasePath(testCasePath);

    return Pinpoint::SUCCESS;
}


