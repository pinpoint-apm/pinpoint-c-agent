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

#ifndef TEST_SIMULATE

#endif

using namespace Pinpoint::log;

//const char* default_ip = "10.34.135.155";
//const char* configFileName = "../conf/pinpoint_collector.conf";
#define DEFAULT_LOGPATH "/tmp"
#define DEFAULT_LOGLEVER "DEBUG"
#define INVALID_PORT -1

//static int32_t collectorSpanPort = INVALID_PORT;
//static int32_t collectorStatPort = INVALID_PORT;
//static int32_t collectorTcpPort = INVALID_PORT;

static void usage();
static int32_t get_conf(const char* configPath);
static int32_t set_quit_signal_handler();
static int32_t block_quit_signal();
static void quit_signal_handler(int signum);

typedef boost::shared_ptr<Pinpoint::Agent::ThreadExecutor> ThreadExecutorPtr;
static ThreadExecutorPtr collectorSpanServerExecutorPtr;
static ThreadExecutorPtr collectorStatServerExecutorPtr;
static ThreadExecutorPtr collectorTcpServerExecutorPtr;

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
        return -1;
    }

    CollectorContext& context = CollectorContext::getContext();

    LOGI("%s", context.toString().c_str());

    err = block_quit_signal();
    if (err != Pinpoint::SUCCESS)
    {
        return -1;
    }

    collectorSpanServerExecutorPtr.reset(new UdpServerExecutor("SpanServer",
                                                               context.getCollectorSpanPort(),
                                                               pinpoint_udp_handler));

    collectorStatServerExecutorPtr.reset(new UdpServerExecutor("statServer",
                                                               context.getCollectorStatPort(),
                                                               pinpoint_udp_handler));

    collectorTcpServerExecutorPtr.reset(new TcpServerExecutor("TcpServer",
                                                              context.getCollectorTcpPort(),
                                                              pinpoint_tcp_handler));

    collectorSpanServerExecutorPtr->start();
    collectorStatServerExecutorPtr->start();
    collectorTcpServerExecutorPtr->start();


    err = set_quit_signal_handler();
    if (err != Pinpoint::SUCCESS)
    {
        return -1;
    }

    collectorSpanServerExecutorPtr->join();
    collectorStatServerExecutorPtr->join();
    collectorTcpServerExecutorPtr->join();
    return 0;
}

void usage()
{
    printf("Usage: test_collector configFilePath.\n");
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


    int32_t collectorSpanPort = config.readUInt32("common.CollectorSpanPort",
                                                          INVALID_PORT);
    if (collectorSpanPort == INVALID_PORT)
    {
        LOGE("invalid collectorSpanPort");
        return Pinpoint::FAILED;
    }
    context.setCollectorSpanPort(collectorSpanPort);

    int32_t collectorStatPort = config.readUInt32("common.CollectorStatPort",
                                                          INVALID_PORT);
    if (collectorStatPort == INVALID_PORT)
    {
        LOGE("invalid collectorStatPort");
        return Pinpoint::FAILED;
    }
    context.setCollectorStatPort(collectorStatPort);

    int32_t collectorTcpPort = config.readUInt32("common.CollectorTcpPort",
                                                         INVALID_PORT);
    if (collectorTcpPort == INVALID_PORT)
    {
        LOGE("invalid collectorTcpPort");
        return Pinpoint::FAILED;
    }
    context.setCollectorTcpPort(collectorTcpPort);

    std::string testCasePath = config.readString("simulate.TestCasePath", "");
    if (testCasePath == "")
    {
        LOGE("invalid testCasePath.")
        return Pinpoint::FAILED;
    }
    context.setTestCasePath(testCasePath);

    return Pinpoint::SUCCESS;
}

int32_t block_quit_signal()
{
    /* register exit check */
    struct sigaction act;
    act.sa_handler = NULL;
    sigemptyset(&act.sa_mask);
    // sigaddset(&act.sa_mask, SIGQUIT);
    sigaddset(&act.sa_mask, SIGQUIT);
    act.sa_flags = 0;
    int32_t err = sigaction(SIGQUIT, &act, 0);
    if (err != 0)
    {
        LOGE("sigaction(SIGQUIT, &act, 0) fail.");
        return Pinpoint::FAILED;
    }

    return Pinpoint::SUCCESS;
}

int32_t set_quit_signal_handler()
{
    /* register exit check */
    struct sigaction act;
    act.sa_handler = quit_signal_handler;
    sigemptyset(&act.sa_mask);
    // sigaddset(&act.sa_mask, SIGQUIT);
    sigaddset(&act.sa_mask, SIGQUIT);
    act.sa_flags = 0;
    int32_t err = sigaction(SIGQUIT, &act, 0);
    if (err != 0)
    {
        LOGE("sigaction(SIGQUIT, &act, 0) fail.");
        return Pinpoint::FAILED;
    }

    return Pinpoint::SUCCESS;
}

void quit_signal_handler(int signum)
{
    if (signum != SIGQUIT)
    {
        LOGE("get wrong signal=%d", signum);
        return;
    }

    LOGI("get exit signal");

    if (collectorSpanServerExecutorPtr != NULL)
    {
        collectorSpanServerExecutorPtr->stop();
    }

    if (collectorStatServerExecutorPtr != NULL)
    {
        collectorStatServerExecutorPtr->stop();
    }

    if (collectorTcpServerExecutorPtr != NULL)
    {
        collectorTcpServerExecutorPtr->stop();
    }
}
