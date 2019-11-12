#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 10/16/19
from CollectorAgent.AgentClient import AgentClient
from CollectorAgent.MetaClient import MetaClient
from CollectorAgent.GrpcSpanFactory import GrpcSpanFactory
from CollectorAgent.SpanClient import SpanClient
from Common.AgentHost import AgentHost
from Common.Logger import TCLogger
from PinpointAgent.PinpointAgent import PinpointAgent
from PinpointAgent.Type import PHP, SUPPORT_GRPC, API_DEFAULT
from Span_pb2 import PSpanMessage, PSpan
from queue import Queue


class GrpcAgentImplement(PinpointAgent):
    def __init__(self, ac, app_id, app_name, serviceType=PHP):

        assert ac.collector_type == SUPPORT_GRPC
        super().__init__(app_id, app_name)
        self.agent_meta = [('starttime',str(ac.startTimestamp)),
                               ('agentid', app_id),
                               ('applicationname',app_name)]
        self.startTimeStamp = ac.startTimestamp
        # maybe change in future
        self.queue = Queue(10000)
        self.app_name = app_name
        self.max_pending_sz = ac.max_pending_size
        self.app_id = app_id
        self.write_queue_ofs = 0
        self.span_buff=([],[])
        self.agent_addr = ac.CollectorAgentIp+':' + str(ac.CollectorAgentPort)
        self.stat_addr = ac.CollectorStatIp + ':' + str(ac.CollectorSpanPort)
        self.span_addr = ac.CollectorSpanIp + ':' + str(ac.CollectorSpanPort)

        import os
        self.agentHost = AgentHost()
        self.agent_client = AgentClient(self.agentHost.hostname, self.agentHost.ip, ac.getWebPort(), os.getpid(), self.agent_addr, self.agent_meta)
        self.meta_client = MetaClient(self.agent_addr, self.agent_meta)
        self.span_client = SpanClient(self._generate_span, self.span_addr, self.agent_meta, self.max_pending_sz)

        self.sequenceId = 0
        self.span_factory = GrpcSpanFactory(self)

    def start(self):
        pass

    def sendSpan(self, stack):
        pSpan = self.span_factory.make_span(stack)
        spanMesg = PSpanMessage(span = pSpan)
        if self.span_client.is_ok:
            self.queue.put(spanMesg)
        elif self.queue.qsize() > self.span_client.max_pending_size:
            TCLogger.warning("span channel is busy. Drop %s",pSpan)
            return
        else:
            self.queue.put(spanMesg)

    def stop(self):
        # close all channel
        self.queue.qsize()

    def _generate_span(self):
        # here return a PSpanMessage
        span = self.queue.get()
        return span

    def updateApiMeta(self,name,type=API_DEFAULT):
        return self.meta_client.update_api_meta(name,-1,type)

    def updateStringMeta(self, name):
        return self.meta_client.update_string_meta(name)

