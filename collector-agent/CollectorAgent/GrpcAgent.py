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

# Created by eeliu at 10/21/19
from grpc import Future

import Service_pb2_grpc
import threading
import time
from CollectorAgent.GrpcClient import GrpcClient
from Common.Logger import TCLogger
from PinpointAgent.Type import PHP
from Stat_pb2 import PAgentInfo, PPing

class GrpcAgent(GrpcClient):
    PINGID=0
    def __init__(self,hostname,ip,ports,pid,address,meta=None,maxPending=-1,timeout=10):
        super().__init__(address,meta,maxPending)
        self.hostname = hostname
        self.ip = ip
        self.pid = pid
        self.stub = Service_pb2_grpc.AgentStub(self.channel)
        self.agentinfo = PAgentInfo(hostname=hostname, ip=ip, ports=ports, pid=pid, endTimestamp=-1,
                               serviceType=PHP)
        self.pingid = GrpcAgent.PINGID
        GrpcAgent.PINGID += 1
        self.ping_meta = meta.append(('socketid', str(GrpcAgent.PINGID)))
        self.timeout = timeout
        self._register()
        self.is_ok = False

    def channel_set_ready(self):
        self.is_ok =True

    def channel_set_idle(self):
        # self._register_agent()
        self.is_ok =False

    def channel_set_error(self):
        self.is_ok =False


    def _register(self):
        # start a thread to handle register
        self.agent_thread = threading.Thread(target=self._registerAgent)
        self.agent_thread.start()


    def _registerAgent(self):
        while True:
            try:
                self.stub.RequestAgentInfo(self.agentinfo, wait_for_ready=True)
            except Exception as e:
                TCLogger.warn("pinpoint collector is not available")
                continue

            iter_response = self.stub.PingSession(self._pingPPing(), metadata=self.ping_meta)
            try:
                for response in iter_response:
                    TCLogger.debug('get ping response %s', response)
            except Exception as e:
                TCLogger.error("ping response abort with exception %s", e)
                time.sleep(self.timeout)

    def reponseAgentInfoCallback(self, future):
        if future.exception():
            TCLogger.error("agent catch exception %s.",future.exception())
            return

        if future.result():
            TCLogger.debug("agent register done:%s",future.result())
            self.is_ok = True
            self._startPingThread()

    def _pingPPing(self):
        while self.is_ok:
            ping = PPing()
            TCLogger.debug("%s send ping %d",self,threading.get_ident())
            yield ping
            time.sleep(self.timeout)


    def _startPingThread(self):
        # create ping stub
        iter_reponse = self.stub.PingSession(self._pingPPing(), metadata=self.ping_meta)
        self.ping_reponse_task = threading.Thread(target=self._pingResponse, args=(iter_reponse,))
        self.ping_reponse_task.start()

    def _pingResponse(self, response_iter):
        try:
            for response in response_iter:
                TCLogger.debug('get ping response %s',response)
        except Exception as e:
            TCLogger.error("ping response abort with exception %s",e)
            self._registerAgent()

    def __str__(self):
        return 'agentclient: hostname:%s ip:%s  pid:%d address:%s'%(self.hostname,self.ip,self.pid,self.address)