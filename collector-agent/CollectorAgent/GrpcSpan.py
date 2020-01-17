#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 10/31/19
from queue import Empty

import Service_pb2_grpc
from CollectorAgent.GrpcClient import GrpcClient
from Common.Logger import TCLogger
import time

from Span_pb2 import PSpan


class GrpcSpan(GrpcClient):
    def __init__(self,address,meta=None):
        super().__init__(address, meta)

        # self.is_ok = True
        self.span_stub = Service_pb2_grpc.SpanStub(self.channel)
        self.span_count = 0
        self.is_ok = False
        self.droped_span_count = 0
        self.log_span_transit_count= 0

    def channel_set_ready(self):
        self.is_ok = True


    def channel_set_idle(self):
        self.is_ok = False

    def channel_set_error(self):
        self.is_ok = False

    def startSender(self,queue):
        spans = []
        def get_N_span(queue,N):
            i = 0
            try:
                while N > i:
                    spans.append(queue.get(timeout=2))
                    i+=1
            except Empty as e:
                TCLogger.debug("get span from queue timeout")
            return True if i>0 else False

        while True:
            try:
                if not get_N_span(queue,10240):
                    continue
                self.span_stub.SendSpan(iter(spans))
            except Exception as e:
                TCLogger.error("span channel, can't work:exception:%s",e)
                time.sleep(10)
            finally:
                spans.clear()




    #
    # def sendSpan(self, spanMesg):
    #     if self.is_ok:
    #         if self.queue.qsize() > self.max_pending_size:
    #             # TCLogger.warning("span channel is busy. Drop span count =%d", self.droped_span_count)
    #             # self.droped_span_count+=1
    #
    #             # return False
    #             self.startSpan()
    #
    #         self.queue.put(spanMesg)
    #         TCLogger.debug("span channel is OK,msg:%d", self.queue.qsize())
    #         return True
    #     elif self.span_channel_wait == True:
    #         return False
    #     else:
    #         # Channel is not available,try to wait and try again
    #         TCLogger.info("span channel is crash, sleep:%sS and try to reconnect", self.span_recover_time_out)
    #         self.timer = self.loop.timer(self.span_recover_time_out)
    #         self.timer.start(self.startSpan)
    #         self.span_channel_wait = True
    #         return False
    #
    #
    # def _spanResponse(self, result):
    #     TCLogger.debug("send span is over. As %s",result)
    #     self.is_ok = False
    #     self.span_channel_wait = False
    #
    # def _getSpan(self):
    #     TCLogger.debug("stream get span, self id: %d",threading.get_ident())
    #     while True:
    #         span = self._pullSpanQueue()
    #         try:
    #             if span is not None:
    #                 self.span_count+=1
    #                 TCLogger.debug("send span count:%d",self.span_count)
    #                 yield span
    #             else:
    #                 TCLogger.error("get span failed, checking the _generate_span")
    #                 break
    #         except Exception as e:
    #             TCLogger.error("get_span catch %s",e)
    #
    # def _pullSpanQueue(self):
    #     span = self.queue.get()
    #     return span
    #
    # def startSpan(self):
    #
    #     # self.future = self.span_stub.SendSpan.future(self._getSpan())
    #     # self.is_ok = True
    #     # self.future.add_done_callback(self._spanResponse)
    #     TCLogger.debug("start span address: %s",self.address)
    #     thread = threading.Thread(target=self._getSpan,args={})
    #     thread.setName("span thread")
    #     thread.start()
    #     self.span_threads.append(thread)