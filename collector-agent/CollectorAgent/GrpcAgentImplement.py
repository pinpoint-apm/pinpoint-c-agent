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
        self.span_queue = Queue(10000)
        self.max_span_sender_size = 10
        self.span_process = []
        self._startSpanSenderProcess()

        self.agent_client = GrpcAgent(self.agentHost.hostname, self.agentHost.ip, ac.getWebPort(), os.getpid(), self.agent_addr, self.agent_meta)
        self.meta_client = GrpcMeta(self.agent_addr, self.agent_meta)

        self.span_factory = GrpcSpanFactory(self)

    def start(self):
        pass


    def sendSpan(self, stack):
        pSpan = self.span_factory.make_span(stack)
        spanMesg = PSpanMessage(span = pSpan)

        try:
            self.span_queue.put(spanMesg,False)
            TCLogger.debug("inqueue size:%d",self.span_queue.qsize())
        except Exception as e:
            TCLogger.info("send queue is full,span process count:%s",len(self.span_process))
            if len(self.span_process)< self.max_span_sender_size:
                self._startSpanSenderProcess()
            else:
                TCLogger.warn("span_processes extend to max size")

        return True

    def _startSpanSenderProcess(self):

        def spanSenderMain(queue):

            # stop gevent
            # try_to_recycle_the_event_hub()

            # start grpcspan
            span_client = GrpcSpan(self.span_addr, self.agent_meta, self.max_pending_sz)
            span_client.startSender(queue)

        sender_process = Process(target=spanSenderMain,args=(self.span_queue,))
        sender_process.name = "span sender process"
        sender_process.start()
        self.span_process.append(sender_process)
        TCLogger.info("Successfully create a Span Sender")


    def stop(self):

        for pro in self.span_process:
            assert isinstance(pro,Process)
            pro.terminate()
        for pro in self.span_process:
            assert isinstance(pro, Process)
            pro.join()


    def updateApiMeta(self,name,type=API_DEFAULT):
        return self.meta_client.update_api_meta(name,-1,type)

    def updateStringMeta(self, name):
        return self.meta_client.update_string_meta(name)

