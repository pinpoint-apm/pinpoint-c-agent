/*******************************************************************************
 * Copyright 2018 NAVER Corp.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License.  You may obtain a copy
 * of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations under
 * the License.
 ******************************************************************************/
#ifndef PINPOINT_SIMULATE_COLLECTOR_CONTEXT_H
#define PINPOINT_SIMULATE_COLLECTOR_CONTEXT_H

#include <stdint.h>
#include <string>

class CollectorContext
{
public:
    int32_t getCollectorSpanPort() const
    {
        return collectorSpanPort;
    }

    void setCollectorSpanPort(int32_t collectorSpanPort)
    {
        this->collectorSpanPort = collectorSpanPort;
    }

    int32_t getCollectorStatPort() const
    {
        return collectorStatPort;
    }

    void setCollectorStatPort(int32_t collectorStatPort)
    {
        this->collectorStatPort = collectorStatPort;
    }

    int32_t getCollectorTcpPort() const
    {
        return collectorTcpPort;
    }

    void setCollectorTcpPort(int32_t collectorTcpPort)
    {
        this->collectorTcpPort = collectorTcpPort;
    }

    const std::string &getConfigPath() const
    {
        return configPath;
    }

    void setConfigPath(const std::string &configPath)
    {
        this->configPath = configPath;
    }

    const std::string &getTestCasePath() const
    {
        return testCasePath;
    }

    void setTestCasePath(const std::string &testCasePath)
    {
        this->testCasePath = testCasePath;
    }

    static CollectorContext& getContext()
    {
        return context;
    }

    std::string toString();

private:
    int32_t collectorSpanPort;
    int32_t collectorStatPort;
    int32_t collectorTcpPort;
    std::string configPath;
    std::string testCasePath;
    static CollectorContext context;
};

#endif
