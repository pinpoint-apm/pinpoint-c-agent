#!/usr/bin/env python
# -*- coding: UTF-8 -*-


# ------------------------------------------------------------------------------
#  Copyright  2020. NAVER Corp.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
# ------------------------------------------------------------------------------

from PHPAgent import PHPAgentConf, FrontAgent
from PinpointAgent.AppManagement import AppManagement
from CollectorAgentConf import config
from Common.Config import CAConfig
_app_management = AppManagement(config)
_pac = PHPAgentConf(CAConfig)
_agent = FrontAgent(_pac, _app_management.handleFrontAgentData)
_agent.registerPHPAgentHello(_app_management.tellMeWho)


def start_pinpoint_agent():
    _agent.start()



def stop_pinpoint_agent():
    _app_management.stopAll()
    _agent.stop()

"""
stop_front_agent: works on subprocess, free all front res before running
"""
def stop_front_agent():
    _agent.stop()


__all__=['start_pinpoint_agent', 'stop_pinpoint_agent', 'stop_front_agent']