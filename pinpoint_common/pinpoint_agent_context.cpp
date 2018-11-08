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
    }
}

