#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 10/16/19
from CollectorAgent.AgentClient import AgentClient
from CollectorAgent.MetaClient import MetaClient
from CollectorAgent.SpanClient import SpanClient
from Common.AgentHost import AgentHost
from PinpointAgent.PinpointAgent import PinpointAgent
from PinpointAgent.Type import PHP, SUPPORT_GRPC
from Span_pb2 import PSpanMessage
from queue import Queue


class GrpcAgentImplement(PinpointAgent):
    def __init__(self, ac, app_id, app_name, serviceType=PHP):

        assert ac.collector_type == SUPPORT_GRPC
        super().__init__(app_id, app_name)
        self.agent_meta = [('starttime',ac.startTimestamp),
                               ('agentid', app_id),
                               ('applicationname',app_name)]
        # maybe change in future
        self.app_name = app_name
        self.max_pending_sz = ac.max_pending_size
        self.app_id = app_id
        self.write_queue_ofs = 0
        self.span_buff=([],[])
        self.agent_addr = ac.CollectorAgentIp+':' + str(ac.CollectorAgentPort)
        self.stat_addr = ac.CollectorStatIp + ':' + str(ac.CollectorSpanPort)
        self.span_addr = ac.CollectorSpanIp + ':' + str(ac.CollectorSpanPort)
        import os
        ah = AgentHost()
        self.agent_client = AgentClient(ah.hostname,ah.ip,ac.getWebPort(),os.getpid(),self.agent_addr,self.agent_meta)
        self.meta_client = MetaClient(self.agent_addr, self.agent_meta)
        self.span_client = SpanClient(self._generate_span, self.span_addr, self.agent_meta, self.max_pending_sz)
        self.queue = Queue(10000)
        self.sequenceId = 0

    def start(self):
        pass

    def sendSpan(self, stack):
        # stack -> grpc-span

        # create stringid,
        pass


    def stop(self):
        # close all channel
        raise NotImplementedError()

    def _generate_span(self):
        # here return a PSpanMessage
        span = self.queue.get()
        assert isinstance(span,PSpanMessage)
        return span