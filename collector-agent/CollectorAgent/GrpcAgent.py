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

import queue
import threading
import time

import grpc

import Service_pb2_grpc
from CollectorAgent.GrpcClient import GrpcClient
from Common.Logger import TCLogger
from PinpointAgent.Type import PHP
from Proto.grpc.Cmd_pb2 import PCmdMessage, PCmdServiceHandshake, PCommandType, PCmdActiveThreadCountRes, \
    PCmdStreamResponse, PCmdActiveThreadLightDumpRes
from Stat_pb2 import PAgentInfo, PPing


class GrpcAgent(GrpcClient):
    PINGID = 0

    def __init__(self, hostname, ip, ports, pid, address, server_type=PHP, meta=None, get_req_stat=None,
                 maxPending=-1, timeout=300, isContainer=False):
        super().__init__(address, meta, maxPending)
        self.hostname = hostname
        self.ip = ip
        self.pid = pid
        self.is_ok = False
        self.timeout = timeout
        self.pingid = GrpcAgent.PINGID
        GrpcAgent.PINGID += 1
        self.exit_cv  = threading.Condition()
        self.stub = Service_pb2_grpc.AgentStub(self.channel)
        self.cmd_sub = Service_pb2_grpc.ProfilerCommandServiceStub(self.channel)
        self.agentinfo = PAgentInfo(hostname=hostname, ip=ip, ports=ports, pid=pid, endTimestamp=-1,
                                    serviceType=server_type, container=isContainer)
        self.ping_meta = meta.append(('socketid', str(GrpcAgent.PINGID)))
        self.profile_meta = meta
        self.task_running = False
        assert get_req_stat
        self.get_req_stat = get_req_stat

    def channelSetReady(self):
        self.is_ok = True

    def channelSetIdle(self):
        self.is_ok = False

    def channelSetError(self):
        self.is_ok = False

    def start(self):
        self.task_running = True

        self.agent_thread = threading.Thread(target=self._registerAgent)
        self.agent_thread.start()

        self.cmd_thread = threading.Thread(target=self._handleCommand)
        self.cmd_thread.start()

    class HandStreamIterator(object):
         def __init__(self,start_msg):
            self._response_queue = queue.Queue()
            self._response_queue.put(start_msg)

         def __next__(self):
             response = self._response_queue.get()
             if response:
                return response
             else:
                TCLogger.info("HandStreamIterator stopped")
                raise StopIteration()
         def add_response(self,message):
             self._response_queue.put(message)

         def stop(self):
             self._response_queue.put(None)

    def _handleCommand(self):
        handshake = PCmdServiceHandshake()
        handshake.supportCommandServiceKey.append(PCommandType.PING)
        handshake.supportCommandServiceKey.append(PCommandType.PONG)
        handshake.supportCommandServiceKey.append(PCommandType.ACTIVE_THREAD_COUNT)
        handshake.supportCommandServiceKey.append(PCommandType.ACTIVE_THREAD_DUMP)
        handshake.supportCommandServiceKey.append(PCommandType.ACTIVE_THREAD_LIGHT_DUMP)
        cmd = PCmdMessage(handshakeMessage=handshake)

        while self.task_running:
            self.cmd_pipe = GrpcAgent.HandStreamIterator(cmd)
            # while self.task_running:
            msg_iter = self.cmd_sub.HandleCommand(self.cmd_pipe, metadata=self.profile_meta)
            try:
                for msg in msg_iter:
                    TCLogger.debug("command channel %s", msg)
                    self._handleCmd(msg, self.cmd_pipe)
                TCLogger.debug('iter_response is over')

            except Exception as e:
                TCLogger.error("handleCommand channel  %s error", e)
            finally:
                with self.exit_cv:
                    if not self.task_running or not self.exit_cv.wait(self.timeout):
                        break

    def _send_thread_count(self,requestId):
        channel = grpc.insecure_channel(self.address)
        stub  = Service_pb2_grpc.ProfilerCommandServiceStub(channel)

        def generator_cmd():
            i = 0
            while self.task_running:
                try:
                    cmd_response = PCmdStreamResponse(responseId=requestId, sequenceId=i)
                    cmd_response.message.value = 'hello'
                    threadCountRes = PCmdActiveThreadCountRes(commonStreamResponse=cmd_response)
                    threadCountRes.histogramSchemaType = 2
                    for stat in self.get_req_stat():
                        threadCountRes.activeThreadCount.append(stat)
                    threadCountRes.timeStamp = int(time.time())
                    i += 1
                    yield threadCountRes
                    ## it's a templated task, don't use exit_cv
                    time.sleep(1)
                except Exception as e:
                    TCLogger.warning("catch exception %s", e)
                    break

        try:
            TCLogger.debug("new a thread for activeThreadCound %d", requestId)
            stub.CommandStreamActiveThreadCount(generator_cmd(),metadata=self.profile_meta)
            TCLogger.debug("send req state requestId: %d done",requestId)
            channel.close()
        except Exception as e:
            TCLogger.error("CommandStreamActiveThreadCount, catch exception %s",e)
    def _handleCmd(self,msg,cmdIter):
        try:
            if msg.HasField('commandEcho'):
                pass
            elif msg.HasField('commandActiveThreadCount'):
                self.thread_count = threading.Thread(target=self._send_thread_count,args=(msg.requestId,))
                self.thread_count.start()
            elif msg.HasField('commandActiveThreadDump'):
                pass

            elif msg.HasField('commandActiveThreadLightDump'):
                lightDumpRes =  PCmdActiveThreadLightDumpRes()
                lightDumpRes.commonResponse.responseId = msg.requestId
                lightDumpRes.type='java'
                lightDumpRes.subType = 'oracle'
                lightDumpRes.version = '1.8.105'
                self.cmd_sub.CommandActiveThreadLightDump(lightDumpRes)
            else:
                TCLogger.warn("msg type not support:%s",msg)
        except Exception as e:
            TCLogger.error(e)


    def _registerAgent(self):
        while self.task_running:
            try:
                TCLogger.debug("sending agentinfo %s",self.agentinfo)
                self.stub.RequestAgentInfo(self.agentinfo)
            except Exception as e:
                TCLogger.warn(" pinpoint collector is not available. Try it again [%s] ",self.agentinfo)
                continue
            finally:
                with self.exit_cv:
                    if not self.task_running or not self.exit_cv.wait(self.timeout):
                        break
            iter_response = self.stub.PingSession(self._pingPPing(), metadata=self.ping_meta)
            try:
                for response in iter_response:
                    TCLogger.debug('get ping response:%s agentinfo:%s', response,self.meta)
            except Exception as e:
                TCLogger.error("ping response abort with exception: [%s]  %s",self.agentinfo, e)

        TCLogger.debug('agent thread exit: %s',self.task_running)


    def _pingPPing(self):
        while self.task_running:
            ping = PPing()
            TCLogger.debug("%s send ping", self)
            yield ping
            with self.exit_cv:
                if not self.task_running or not self.exit_cv.wait(self.timeout):
                    TCLogger.debug("generate ping exit")
                    break


    def __str__(self):
        return 'agentclient: hostname:%s ip:%s  pid:%d address:%s' % (self.hostname, self.ip, self.pid, self.address)

    def stop(self):
        TCLogger.debug("agent thread try to stop")
        self.task_running = False
        with self.exit_cv:
            self.exit_cv.notify_all()
        self.cmd_pipe.stop()
        self.agent_thread.join()
        self.cmd_thread.join()
        super().stop()
        TCLogger.debug("agent thread exit")