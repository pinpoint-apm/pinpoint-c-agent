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
        agentinfo = PAgentInfo(hostname=hostname, ip=ip, ports=ports, pid=pid, endTimestamp=-1,
                               serviceType=PHP)
        self.pingid= AgentClient.PINGID
        AgentClient.PINGID += 1
        self.ping_meta = meta.append(('socketid', str(AgentClient.PINGID)))
        self._register_agent(agentinfo)
        self.ping_timeout =ping_timeout

    def _register_agent(self, agentInfo):
        assert isinstance(agentInfo,PAgentInfo)
        call_future = self.stub.RequestAgentInfo.future(agentInfo)
        callback = partial(self.print_return_unary_mesg,input=agentInfo)
        call_future.add_done_callback(callback)

    def print_return_unary_mesg(self,future,input):
        # start ping if future is success
        if future.result():
            TCLogger.debug("agent register done:%s",future.result())
            self._try_ping_session()

    def _ping_PPing(self):
        while self.state == CH_READY:
            ping = PPing()
            TCLogger.debug("%s send ping",self)
            yield ping
            import time
            time.sleep(10)

    def _try_ping_session(self):
        # create ping stub
        ret_iter = self.stub.PingSession(self._ping_PPing(),metadata=self.ping_meta)
        self.ping_reponse_task = threading.Thread(target=self._ping_response, args=(ret_iter))
        self.ping_reponse_task.start()

    def _ping_response(self,response_iter):
        for response in response_iter:
            TCLogger.debug('get ping response %s',response)

        TCLogger.warn('Agent [%s] ping thread stopped',self)

    def __str__(self):
        return 'hostname:%s ip:%s  pid:%d address:%s'%(self.hostname,self.ip,self.pid,self.address )