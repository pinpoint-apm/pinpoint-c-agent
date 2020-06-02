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

from threading import Thread
from queue import Full,Queue

from CollectorAgent.GrpcAgent import GrpcAgent
from CollectorAgent.GrpcMeta import GrpcMeta
from CollectorAgent.GrpcSpan import GrpcSpan
from CollectorAgent.GrpcSpanFactory import GrpcSpanFactory
from CollectorAgent.GrpcStat import GrpcStat
from Common.AgentHost import AgentHost
from Common.Logger import TCLogger
from PinpointAgent.PinpointAgent import PinpointAgent
from PinpointAgent.Type import PHP, SUPPORT_GRPC, API_DEFAULT
from Span_pb2 import PSpanMessage


class GrpcAgentImplement(PinpointAgent):

    class SpanSender(object):
        def __init__(self, span_addr, appid, appname, starttime,max_pending_sz):
            self.agent_meta = [('starttime', str(starttime)), ('agentid', appid), ('applicationname', appname)]
            self.agent_id = appid
            self.agent_name = appname
            self.span_addr = span_addr
            self.max_pending_sz =max_pending_sz
            self.span_queue = Queue(self.max_pending_sz)
            self.span_client = GrpcSpan(self.span_addr, self.agent_meta)
            self.dropped_span_count=0
            TCLogger.info("Successfully create a Span Sender")

        def start(self):
            self.span_client.start(self.span_queue)


        def sendSpan(self, spanMesg):

            try:
                self.span_queue.put(spanMesg, False)
            except Full as e:
                self.dropped_span_count+=1
                return False
            except Exception as e:
                TCLogger.error("send span failed: %s", e)
                return False
            return True

        def stopSelf(self):
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

        import os
        self.agentHost = AgentHost()
        self.max_span_sender_size = 2
        self.span_sender_list = []
        self.sender_index = 0
        self._startSpanSender()

        self.agent_client = GrpcAgent(self.agentHost.hostname, self.agentHost.ip, ac.getWebPort(), os.getpid(),
                                      self.agent_addr, self.service_type,self.agent_meta,self.getReqStat)
        self.meta_client = GrpcMeta(self.agent_addr, self.agent_meta)
        self.stat_client = GrpcStat(self.stat_addr,self.agent_meta,self.getIntervalStat)

        self.agent_client.start()
        self.meta_client.start()
        self.stat_client.start()
        self.span_factory = GrpcSpanFactory(self)

    def start(self):
        pass

    def _sendSpan(self, spanMsg):

        self.span_sender_list[0].sendSpan(spanMsg)
        TCLogger.debug(spanMsg)
        return True

    def sendSpan(self, stack, body):
        super().sendSpan(stack,body)
        try:
            pSpan = self.span_factory.makeSpan(stack)
            spanMesg = PSpanMessage(span=pSpan)
        except Exception as e:
            TCLogger.warn("interrupted by %s",e)
            return False
        if self._sendSpan(spanMesg):
            return True

        else:
            if len(self.span_sender_list) < self.max_span_sender_size:
                TCLogger.warn("try to create a new span_sender")
                self._startSpanSender()
            else:
                TCLogger.warn("span_processes extend to max size")

        return True

    def _startSpanSender(self):
        spanSender = GrpcAgentImplement.SpanSender(self.span_addr, self.app_id, self.app_name, self.startTimeStamp,self.max_pending_sz)
        spanSender.start()
        self.span_sender_list.append(spanSender)


    def stop(self):
        self.agent_client.stop()
        self.meta_client.stop()
        self.stat_client.stop()
        for sender in self.span_sender_list:
            sender.stopSelf()

    def updateApiMeta(self, name, type=API_DEFAULT):
        return self.meta_client.updateApiMeta(name, -1, type)

    def updateStringMeta(self, name):
        return self.meta_client.updateStringMeta(name)
