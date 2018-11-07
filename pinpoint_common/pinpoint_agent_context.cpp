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
#include "pinpoint_agent_context.h"
#include "pinpoint_error.h"
#include "pinpoint_api.h"

namespace Pinpoint
{
    namespace Agent
    {

#if 0
        const static AgentConfigArgs defaultConfigArgs = {
                "uninitiated agentid",
                "uninitiated applicationName",
                "127.0.0.1",
                8000,
                "127.0.0.1",
                8001,
                "127.0.0.1",
                8002,
                "/tmp/",
                "DEBUG",
                5,
                false,
                false,
                "",
                "",
                false,
                "",
                -1,
                -1
        };
#endif
        PinpointAgentContextPtr PinpointAgentContext::instance;

        int32_t PinpointAgentContext::initContext()
        {
            try
            {
                PinpointAgentContext::instance.reset(new PinpointAgentContext);
            }
            catch (std::bad_alloc&)
            {
                return OUT_OF_MEMORY;
            }
            return SUCCESS;
        }

        PinpointAgentContextPtr& PinpointAgentContext::getContextPtr()
        {
            return PinpointAgentContext::instance;
        }

#if 0
        bool AgentConfigArgs::assignArgs(Configuration::Config* config)
        {
            const static std::string notSet = "NULL";

            agentId = config->readString("common.AgentID", defaultConfigArgs.agentId);
            applicationName = config->readString("common.ApplicationName",
                    defaultConfigArgs.applicationName);
            logFileRootPath = config->readString("common.LogFileRootPath",
                    defaultConfigArgs.logFileRootPath);
            logLevel = config->readString("common.PPLogLevel",
                    defaultConfigArgs.logLevel);

            collectorSpanIp = config->readString("common.CollectorSpanIp",
                    defaultConfigArgs.collectorSpanIp);
            collectorStatIp = config->readString("common.CollectorStatIp",
                    defaultConfigArgs.collectorStatIp);
            collectorTcpIp = config->readString("common.CollectorTcpIp",
                    defaultConfigArgs.collectorTcpIp);

            collectorSpanPort = config->readUInt32("common.CollectorSpanPort",
                    defaultConfigArgs.collectorSpanPort);
            collectorStatPort = config->readUInt32("common.CollectorStatPort",
                    defaultConfigArgs.collectorStatPort);
            collectorTcpPort = config->readUInt32("common.CollectorTcpPort",
                    defaultConfigArgs.collectorTcpPort);

            pluginInclude = config->readString("common.PluginInclude", notSet);
            pluginIncludeIsSet = pluginInclude != notSet;

            skipTraceTime = config->readInt32("common.SkipTraceTime", -1);
            traceLimit = config->readInt32("common.TraceLimit", -1);

            pluginExclude = config->readString("common.PluginExclude", notSet);
            pluginExcludeIsSet = pluginExclude != notSet;

            apiAssignFileName = config->readString("test.ApiTableFile", notSet);
            apiAssignFileNameIsSet = apiAssignFileName != notSet;

//            pluginFileName = config->readString("PHP.PluginEntryFile",
//                    defaultConfigArgs.pluginFileName);
//            pluginDir = config->readString("PHP.PluginRootDir",
//                    defaultConfigArgs.pluginDir);

            reconInterval = (uint32_t)config->readInt32("common.ReconTimeOut", defaultConfigArgs.reconInterval);

            return true;
        }
#endif

    }
}

