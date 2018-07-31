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
#include "test_env.h"
#include "pinpoint_api.h"

using namespace Pinpoint::Agent;

TEST(agent_test, basic_test)
{
    const char* configFileName = "pinpoint_agent_test.conf";
    Pinpoint::Configuration::ConfFileObject fileobj(configFileName);
    Pinpoint::Configuration::Config config(fileobj);
    int32_t err;


    err = Agent::createAgent();
    ASSERT_EX((err == Pinpoint::SUCCESS), err, Pinpoint::SUCCESS);

    AgentPtr agentPtr = Agent::getAgentPtr();
    assert (agentPtr != NULL);
    ASSERT_EX((agentPtr->getAgentStatus() == AGENT_CREATED), agentPtr->getAgentStatus(), AGENT_CREATED);


    AgentFunction agentFunction;
    agentFunction.logOutputFunc = NULL;
    agentFunction.addInterceptorFunc = NULL;
    agentFunction.getHostProcessInfo = NULL;

    err = agentPtr->preInit(Pinpoint::Agent::PHP_AGENT_TYPE,
                            agentFunction,
                            config);
    ASSERT_EX((err == Pinpoint::SUCCESS), err, Pinpoint::SUCCESS);
    ASSERT_EX((agentPtr->getAgentStatus() == AGENT_PRE_INITED), agentPtr->getAgentStatus(), AGENT_PRE_INITED);

// can't start agent int test environment
//    Pinpoint::Plugin::PluginPtrVector pluginPtrVector;
//    err = agentPtr->init(pluginPtrVector);
//    ASSERT_EX((err == Pinpoint::SUCCESS), err, Pinpoint::SUCCESS);
//    ASSERT_EX((agentPtr->getAgentStatus() == AGENT_INITED), agentPtr->getAgentStatus(), AGENT_INITED);
//
//    err = agentPtr->start();
//    ASSERT_EX((err == Pinpoint::SUCCESS), err, Pinpoint::SUCCESS);
//    ASSERT_EX((agentPtr->getAgentStatus() == AGENT_STARTED), agentPtr->getAgentStatus(), AGENT_STARTED);

    err = agentPtr->stop();
    ASSERT_EX((err == Pinpoint::SUCCESS), err, Pinpoint::SUCCESS);
    ASSERT_EX((agentPtr->getAgentStatus() == AGENT_STOPPED), agentPtr->getAgentStatus(), AGENT_STOPPED);
}
