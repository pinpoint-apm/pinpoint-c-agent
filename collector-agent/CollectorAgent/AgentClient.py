#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 10/21/19
from grpc import Future

import Service_pb2_grpc
import threading
from functools import partial

from CollectorAgent.Grpc import CH_READY
from CollectorAgent.GrpcClient import GrpcClient
from Common.Logger import TCLogger
from PinpointAgent.Type import PHP
from Stat_pb2 import PAgentInfo, PPing

class AgentClient(GrpcClient):
    PINGID=0
    def __init__(self,hostname,ip,ports,pid,address,meta=None,maxPending=-1,ping_timeout=10):
        super().__init__(address,meta,maxPending)
        self.hostname = hostname
        self.ip = ip
        self.pid = pid
        self.stub = Service_pb2_grpc.AgentStub(self.channel)
        self.agentinfo = PAgentInfo(hostname=hostname, ip=ip, ports=ports, pid=pid, endTimestamp=-1,
                               serviceType=PHP)
        self.pingid= AgentClient.PINGID
        AgentClient.PINGID += 1
        self.ping_meta = meta.append(('socketid', str(AgentClient.PINGID)))
        self.ping_timeout = ping_timeout


    def channel_is_ready(self):
        self._start_ping_thread()

    def channel_is_idle(self):
        self._register_agent()

    def channel_is_error(self):
        self._register_agent()

    def _register_agent(self):
        assert isinstance(self.agentinfo,PAgentInfo)
        call_future = self.stub.RequestAgentInfo.future(self.agentinfo)
        callback = partial(self.reponse_agentinfo_callback, input=self.agentinfo)
        call_future.add_done_callback(callback)

    def reponse_agentinfo_callback(self, future, input):
        # start ping if future is success
        if future.exception():
            TCLogger.error("agent catch exception %s",future.exception())
            return

        if future.result():
            TCLogger.debug("agent register done:%s",future.result())


    def _ping_PPing(self):
        while True:
            ping = PPing()
            TCLogger.debug("%s send ping",self)
            yield ping
            import time
            time.sleep(10)

    def _start_ping_thread(self):
        # create ping stub
        iter_reponse = self.stub.PingSession(self._ping_PPing(),metadata=self.ping_meta)
        self.ping_reponse_task = threading.Thread(target=self._ping_response, args=(iter_reponse,))
        self.ping_reponse_task.start()

    def _ping_response(self,response_iter):
        for response in response_iter:
            TCLogger.debug('get ping response %s',response)
        TCLogger.warning('Agent [%s] ping thread stopped',self)

    def __str__(self):
        return 'hostname:%s ip:%s  pid:%d address:%s'%(self.hostname,self.ip,self.pid,self.address )