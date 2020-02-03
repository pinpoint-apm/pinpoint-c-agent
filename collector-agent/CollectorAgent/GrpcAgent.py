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

# Created by eeliu at 10/21/19

import threading
import time

import Service_pb2_grpc
from CollectorAgent.GrpcClient import GrpcClient
from Common.Logger import TCLogger
from PinpointAgent.Type import PHP
from Stat_pb2 import PAgentInfo, PPing


class GrpcAgent(GrpcClient):
    PINGID = 0

    def __init__(self, hostname, ip, ports, pid, address, meta=None, maxPending=-1, timeout=10):
        super().__init__(address, meta, maxPending)
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
        self.task_running = False
        self.is_ok = False

    def channel_set_ready(self):
        self.is_ok = True

    def channel_set_idle(self):
        self.is_ok = False

    def channel_set_error(self):
        self.is_ok = False

    def _register(self):
        # start a thread to handle register
        self.agent_thread = threading.Thread(target=self._registerAgent)
        self.agent_thread.start()

    def _registerAgent(self):
        self.task_running = True
        while self.task_running:
            try:
                self.stub.RequestAgentInfo(self.agentinfo, wait_for_ready=True)
            except Exception as e:
                TCLogger.warn("[%s] pinpoint collector is not available. Try it again",self.agentinfo)
                continue

            iter_response = self.stub.PingSession(self._pingPPing(), metadata=self.ping_meta)
            try:
                for response in iter_response:
                    TCLogger.debug('get ping response:%s agentinfo:%s', response,self.meta)
            except Exception as e:
                TCLogger.error("[%s] ping response abort with exception %s", e,self.agentinfo)
                time.sleep(self.timeout)

    def _pingPPing(self):
        while self.is_ok and self.task_running:
            ping = PPing()
            TCLogger.debug("%s send ping", self)
            yield ping
            time.sleep(self.timeout)

    def __str__(self):
        return 'agentclient: hostname:%s ip:%s  pid:%d address:%s' % (self.hostname, self.ip, self.pid, self.address)

    def stop(self):
        if self.task_running:
            self.task_running = False
        self.agent_thread.join()
        print("agent thread exit")