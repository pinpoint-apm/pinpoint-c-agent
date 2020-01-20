#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 10/31/19
from queue import Empty

import Service_pb2_grpc
from CollectorAgent.GrpcClient import GrpcClient
from Common.Logger import TCLogger
import time
import  traceback



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
                    spans.append(queue.get(timeout=0.005))
                    i+=1
            except Empty as e:
                pass
                # TCLogger.debug("get span from queue timeout")
            return True if i>0 else False

        while True:
            try:
                if not get_N_span(queue,1024):
                    continue
                self.span_stub.SendSpan(iter(spans))
            except Exception as e:
                TCLogger.error("span channel, can't work:exception:%s",e)
                traceback.print_exc()
                time.sleep(10)
            finally:
                spans.clear()