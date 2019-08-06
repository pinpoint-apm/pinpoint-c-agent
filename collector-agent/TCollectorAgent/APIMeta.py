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

from TCollectorAgent.TPackets import *
from Common.Trace.ttypes import TApiMetaData, TStringMetaData
from TCollectorAgent.Type import *
from Common.Logger import TCLogger

class APIMeta(object):
    API_META_INDEX = 1
    def __init__(self,name=None,line = -1,type = API_UNDEFINED,agentStartTime = -1,agentId = -1,agentName='Default name'):
        assert name is not None
        self.name = name
        self.apiId = APIMeta.API_META_INDEX
        APIMeta.API_META_INDEX += 1
        self.type = type
        self.agentStartTime = agentStartTime
        self.agentId = agentId
        self.agentName = agentName

    def toPacket(self):
        apiMeta = TApiMetaData(agentId = self.agentId,
                               agentStartTime =self.agentStartTime,
                               apiId = self.apiId,
                               type= self.type,
                               apiInfo=self.name)
        TCLogger.debug("generator APIMeta:%s", apiMeta)
        body   = CollectorPro.obj2bin(apiMeta, API_META_DATA)
        packet = Packet(PacketType.APPLICATION_REQUEST,CollectorPro.getCurReqCount() ,len(body),body)
        return packet


class StringMetaData(object):
    API_META_INDEX= 1
    PHP_STRING_MAP = {
        "EXP": "Exception:",
        "ERR": "Fatal error:"
    }
    def __init__(self,agentId,agentStartTime,name):
        self.agentId = agentId
        self.agentStartTime = agentStartTime

        if name in StringMetaData.PHP_STRING_MAP:
            meta_str = self.PHP_STRING_MAP[name]
        else:
            meta_str = "collector-agent needs upgrade"

        self.value = meta_str
        self.apiId = StringMetaData.API_META_INDEX
        self.name = name
        StringMetaData.API_META_INDEX +=1

    def toPacket(self):
        stringMeta = TStringMetaData(agentId=self.agentId,
                                     agentStartTime= self.agentStartTime,
                                     stringId=self.apiId,
                                     stringValue=self.value)
        TCLogger.debug("generator stringMate:%s", stringMeta)
        body   = CollectorPro.obj2bin(stringMeta,STRING_META_DATA)
        packet = Packet(PacketType.APPLICATION_REQUEST,CollectorPro.getCurReqCount() ,len(body),body)
        return packet





class InterceptManager(object):
    def __init__(self, collector_cb, ac):
        '''

        :param tcpLayer:
        :param CollectorAgentConf ac:
        '''
        self.collector_cb   = collector_cb
        self.api_metas = {}
        self.string_metas = {}
        self.startTimeStamp = ac.startTimestamp
        self.ac = ac

    def sendMeta(self, meta):
        '''

        :param APIMeta meta:
        :return:
        '''
        TCLogger.debug("meta: %s", meta.name)
        self.collector_cb(meta.toPacket().getSerializedData())

    def updateStringMeta(self,name):

        if name in self.string_metas:
            return self.string_metas[name]
        else:

            meta = StringMetaData(agentStartTime =self.startTimeStamp, agentId=self.ac.AgentID,name=name)
            self.sendMeta(meta)
            self.string_metas[name] = meta
            return meta

    def updateApiMeta(self, name, api_type=API_DEFAULT):

        if name in self.api_metas:
            return self.api_metas[name]
        else:
            meta = APIMeta(name=name, type = api_type,
                                agentStartTime =self.startTimeStamp, agentId=self.ac.AgentID, agentName=self.ac.ApplicationName)
            self.sendMeta(meta)
            self.api_metas[name] = meta
            return meta


# interceptManger = InterceptManager()
