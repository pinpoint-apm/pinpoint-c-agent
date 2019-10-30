#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 10/16/19
import time

import grpc

from CollectorAgent.Grpc import CH_NOT_READY, CH_READY
from PinpointAgent import PinpointAgent
from PinpointAgent.Type import PHP, SUPPORT_GRPC
from Stat_pb2 import PPing


class GrpcAgentImplement(PinpointAgent):
    socketUniqueId=0
    def __init__(self, manage, ac, app_id, app_name, serviceType=PHP):
        from CollectorAgent import CollectorAgentConf
        assert(isinstance(ac,CollectorAgentConf))
        assert(ac.collector_type == SUPPORT_GRPC)
        super().__init__(app_id, app_name)
        self.agent_metadata = [('starttime',self.manage.startTimestamp),
                               ('agentid', app_id),
                               ('applicationname',app_name)]
        self.span_metadata = self.agent_metadata.append(())
        self.ping_metadata = self.agent_metadata.append(('socketid',str(GrpcAgentImplement.socketUniqueId)))
        GrpcAgentImplement.socketUniqueId += 1
        # maybe change in future
        self.app_name = app_name
        self.max_pending_sz = ac.max_pending_size
        self.app_id = app_id
        self.span_c_state = CH_NOT_READY
        self.stat_c_state = CH_NOT_READY
        self.agent_c_state = CH_NOT_READY
        self.write_queue_ofs = 0
        self.span_buff=([],[])
        self.agent_addr = ac.CollectorAgentIp+':' + str(ac.CollectorAgentPort)
        self.stat_addr = ac.CollectorStatIp + ':' + str(ac.CollectorSpanPort)
        self.span_addr = ac.CollectorSpanIp + ':' + str(ac.CollectorSpanPort)
        self.agent_channel,self.stat_channel,self.span_channel = self._initialize_channels()

    def _initialize_channels(self):
        agent_ch = grpc.insecure_channel(self.agent_addr)
        stat_ch = grpc.insecure_channel(self.stat_addr)
        span_ch = grpc.insecure_channel(self.span_addr)

        # add state change hook



        return agent_ch,stat_ch,span_ch


    def set_span_channel_state(self,state):
        pass

    def set_agent_channel_state(self,state):
        pass

    def set_stat_channel_state(self,state):
        pass


    def start(self):
        # 1. create channel of span,agent,stat
        # 2. register agent
        # 3. keep ping
        raise NotImplementedError()

    def sendSpan(self, stack):
        # stack -> grpc-span

        # create stringid,

        # elapsed by 1s
        raise NotImplementedError()

    def stop(self):
        # close all channel
        raise NotImplementedError()