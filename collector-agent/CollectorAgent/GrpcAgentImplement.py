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

# Created by eeliu at 10/16/19
import json
import os
from multiprocessing import Process, Queue as MPQueue
from queue import Full, Queue

from CollectorAgent.GrpcAgent import GrpcAgent
from CollectorAgent.GrpcMeta import GrpcMeta
from CollectorAgent.GrpcSpan import GrpcSpan
from CollectorAgent.GrpcSpanFactory import GrpcSpanFactory
from CollectorAgent.GrpcStat import GrpcStat
from Common.AgentHost import AgentHost
from Common.Logger import TCLogger
from PinpointAgent.PinpointAgent import PinpointAgent
from PinpointAgent.Type import SUPPORT_GRPC, API_DEFAULT
from Proto.grpc.Span_pb2 import PSpanMessage


class GrpcAgentImplement(PinpointAgent):
    class SpanHelper(object):
        def __init__(self, span_addr, appid, appname, starttime,max_pending_sz):
            self.agent_meta = [('starttime', str(starttime)), ('agentid', appid), ('applicationname', appname)]
            self.agent_id = appid
            self.agent_name = appname
            self.span_addr = span_addr
            self.max_pending_sz =max_pending_sz
            self.span_queue = Queue(self.max_pending_sz)
            self.span_client = GrpcSpan(self.span_addr, self.agent_meta, self.span_queue)
            self.dropped_span_count=0

        def start(self):
            self.span_client.start()


        def sendSpan(self, spanMesg):

            try:
                self.span_queue.put(spanMesg, False)
            except Full as e:
                self.dropped_span_count+=1
                TCLogger.warning("span send queue is full")
                return False
            except Exception as e:
                TCLogger.error("send span failed: %s", e)
                return False
            return True

        def stop(self):
            self.span_client.stop()
            TCLogger.info("grpc agent dropped %d",self.dropped_span_count)

    def __init__(self, ac, app_id, app_name, serviceType):
        assert ac.collector_type == SUPPORT_GRPC
        super().__init__(app_id, app_name)
        self.agent_meta = [('starttime', str(ac.startTimestamp)),
                           ('agentid', app_id),
                           ('applicationname', app_name)]
        self.startTimeStamp = ac.startTimestamp
        self.service_type = serviceType
        self.max_pending_sz = ac.max_pending_size
        self.agent_addr = ac.CollectorAgentIp + ':' + str(ac.CollectorAgentPort)
        self.stat_addr = ac.CollectorStatIp + ':' + str(ac.CollectorStatPort)
        self.span_addr = ac.CollectorSpanIp + ':' + str(ac.CollectorSpanPort)
        self.web_port = ac.getWebPort()
        self.agentHost = AgentHost()
        self.max_span_sender_size = 2
        self.sender_index = 0

    def start(self):
        self.mpQueue = MPQueue()
        self.process = Process(target=self.processMain)
        self.process.start()

    def processMain(self):
        self.span_helper = GrpcAgentImplement.SpanHelper(self.span_addr, self.app_id, self.app_name,
                                                         self.startTimeStamp,
                                                         self.max_pending_sz)
        self.agent_client = GrpcAgent(self.agentHost.hostname, self.agentHost.ip, self.web_port, os.getpid(),
                                      self.agent_addr, self.service_type,self.agent_meta,self.getReqStat)
        self.meta_client = GrpcMeta(self.agent_addr, self.agent_meta)
        self.stat_client = GrpcStat(self.stat_addr,self.agent_meta,self.getIntervalStat)
        self.span_factory = GrpcSpanFactory(self)
        self.agent_client.start()
        self.meta_client.start()
        self.stat_client.start()
        self.span_helper.start()

        self.loopTheQueue()

        self.stopProcessMain()

    def loopTheQueue(self):
        while True:
            try:
                body = self.mpQueue.get()
            except KeyboardInterrupt:
                break
            if body == None:
                TCLogger.info("agent: %s stopping", self.agent_meta)
                break
            else:
                content = body.decode('utf-8')
                try:
                    TCLogger.debug(content)
                    stack = json.loads(content)
                except Exception as e:
                    TCLogger.error("json is crash")
                    return

                super().sendSpan(stack, body)
                try:
                    pSpan = self.span_factory.makeSpan(stack)
                    spanMesg = PSpanMessage(span=pSpan)
                except Exception as e:
                    TCLogger.warn("interrupted by %s", e)
                    continue
                self.span_helper.sendSpan(spanMesg)

    def asynSendSpan(self, stack, body):
        self.mpQueue.put(body, 5)

    def stop(self):
        self.mpQueue.put(None, 5)
        self.process.join()

    def stopProcessMain(self):
        self.agent_client.stop()
        self.meta_client.stop()
        self.stat_client.stop()
        self.span_helper.stop()

    def updateApiMeta(self, name, type=API_DEFAULT):
        return self.meta_client.updateApiMeta(name, -1, type)

    def updateStringMeta(self, name):
        return self.meta_client.updateStringMeta(name)

    def updateSqlMeta(self, sql):
        return self.meta_client.updateSqlMeta(sql)
