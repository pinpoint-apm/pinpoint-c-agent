#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 10/16/19
from queue import Full


from CollectorAgent.GrpcAgent import GrpcAgent
from CollectorAgent.GrpcMeta import GrpcMeta
from CollectorAgent.GrpcSpanFactory import GrpcSpanFactory
from CollectorAgent.GrpcSpan import GrpcSpan
from Common.AgentHost import AgentHost
from Common.Logger import TCLogger
from Events.util import try_to_recycle_the_event_hub
from PinpointAgent.PinpointAgent import PinpointAgent
from PinpointAgent.Type import PHP, SUPPORT_GRPC, API_DEFAULT
from Span_pb2 import PSpanMessage
from multiprocessing import Process,Queue

class GrpcAgentImplement(PinpointAgent):

    class SpanSender(object):
        def __init__(self,span_addr,agent_meta):
            self.span_addr, self.agent_meta = (span_addr,agent_meta)
            self.span_queue = Queue(5000)
            self.sender_process = Process(target=self.spanSenderMain, args=(self.span_queue,))
            self.sender_process.name = "span sender process"

            TCLogger.info("Successfully create a Span Sender")

        def start(self):
            self.sender_process.start()

        def sender(self,spanMesg):

            try:
                self.span_queue.put(spanMesg,False)
                TCLogger.debug("inqueue size:%d", self.span_queue.qsize())
            except Full as e:
                TCLogger.error("send span failed: with queue is FUll%s", e)
                return False
            except Exception as e:
                TCLogger.error("send span failed: %s",e)
                return False;
            return True

        def stopSelf(self):
            self.sender_process.terminate()
            self.sender_process.join()

        def spanSenderMain(self,queue):

            span_client = GrpcSpan(self.span_addr, self.agent_meta)
            span_client.startSender(queue)


    def __init__(self, ac, app_id, app_name, serviceType=PHP):

        assert ac.collector_type == SUPPORT_GRPC
        super().__init__(app_id, app_name)
        self.agent_meta = [('starttime',str(ac.startTimestamp)),
                               ('agentid', app_id),
                               ('applicationname',app_name)]
        self.startTimeStamp = ac.startTimestamp

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
        self.max_span_sender_size = 2
        self.span_sender_list = []
        self.sender_index= 0
        self._startSpanSender()
        self.iter_sender_list = iter(self.span_sender_list)

        self.agent_client = GrpcAgent(self.agentHost.hostname, self.agentHost.ip, ac.getWebPort(), os.getpid(), self.agent_addr, self.agent_meta)
        self.meta_client = GrpcMeta(self.agent_addr, self.agent_meta)

        self.span_factory = GrpcSpanFactory(self)

    def start(self):
        pass


    def _sendSpan(self,spanMesg):

        max_checking = len(self.span_sender_list)

        while self.span_sender_list[self.sender_index].sender(spanMesg) == False:
            TCLogger.debug("sender index %d is busy",self.sender_index)
            self.sender_index += 1
            if self.sender_index >= len(self.span_sender_list):
                self.sender_index = 0
            max_checking -=1
            if max_checking == 0:
                return False

        self.sender_index += 1
        if self.sender_index >= len(self.span_sender_list):
            self.sender_index = 0

        return True

    def sendSpan(self, stack):
        pSpan = self.span_factory.make_span(stack)
        spanMesg = PSpanMessage(span = pSpan)
        if self._sendSpan(spanMesg):
            return True
        else:
            TCLogger.info("send is block, try to create span process count:%s",len(self.span_sender_list))
            if len(self.span_sender_list)< self.max_span_sender_size:
                self._startSpanSender()
            else:
                TCLogger.warn("span_processes extend to max size")

        return True

    def _startSpanSender(self):
        spanSender = GrpcAgentImplement.SpanSender(self.span_addr, self.agent_meta)
        spanSender.start()
        self.span_sender_list.append(spanSender)



    def stop(self):
        for sender in self.span_sender_list:
            sender.stopSelf()


    def updateApiMeta(self,name,type=API_DEFAULT):
        return self.meta_client.update_api_meta(name,-1,type)

    def updateStringMeta(self, name):
        return self.meta_client.update_string_meta(name)

