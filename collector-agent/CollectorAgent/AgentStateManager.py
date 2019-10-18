#-------------------------------------------------------------------------------
# Copyright 2019 NAVER Corp
# 
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License.  You may obtain a copy
# of the License at
# 
#   http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations under
# the License.
#-------------------------------------------------------------------------------
#!/usr/bin/env python
# -*- coding: UTF-8 -*-
import psutil

from CollectorAgent.Protocol import CollectorPro
from PinpointAgent.Type import AGENT_STAT_BATCH
from Common import TCLogger
from Proto.Trift.Pinpoint.ttypes import TAgentStat, TCpuLoad, TAgentStatBatch

from Trains import DgramLayer, TrainLayer



class AgentStateManager(object):
    def __init__(self, agentId,startTimeStamp,host):
        self.state=TAgentStat()
        self.cup_load=TCpuLoad()
        self.state.agentId = agentId
        self.state.startTimestamp = startTimeStamp
        self.stateBatch = TAgentStatBatch()
        self.stateBatch.agentId = agentId
        self.stateBatch.startTimestamp = startTimeStamp
        self.stateBatch.agentStats  = []
        self.remote = host

        self.trans_layer = DgramLayer(host, None)
        self.trans_layer.start()

        TrainLayer.registerTimers(self.sendState,20)
        TCLogger.debug("register state timer")

    def _upDateCurState(self):
        self.stateBatch.agentStats = []
        ## cpu
        self.cup_load.systemCpuLoad = psutil.getloadavg()[0]
        self.state.cpuLoad = self.cup_load

        self.stateBatch.agentStats.append(self.state)

    def sendState(self,layer):
        self._upDateCurState()
        body = CollectorPro.obj2bin(self.stateBatch, AGENT_STAT_BATCH)
        TCLogger.debug("send state:%s",self.stateBatch)
        self.trans_layer.sendData(body)


