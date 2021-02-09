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

# Created by eeliu at 10/31/19
from concurrent.futures.thread import ThreadPoolExecutor
from queue import Empty
from threading import Condition

import grpc

import Service_pb2_grpc
from CollectorAgent.GrpcClient import GrpcClient
from Common.Logger import TCLogger
from Common.util import profile


class GrpcSpan:
    class SpanClient:
        def __init__(self, address, meta):
            self.address = address
            self.meta = meta
            self._connect()

        def _connect(self):
            TCLogger.info("span channel connect %s with meta:%s", self.address, self.meta)
            self.channel = grpc.intercept_channel(grpc.insecure_channel(self.address),
                                                  GrpcClient.InterceptorAddHeader(self.meta))
            self.span_stub = Service_pb2_grpc.SpanStub(self.channel)

        @profile
        def sendSpans(self, iter):
            self.span_stub.SendSpan(iter, timeout=5)

        def reconnect(self):
            self.channel.close()
            self._connect()


    def __init__(self, address, meta, queue):
        self.address = address
        self.meta = meta
        self.exit_cv = Condition()
        self.send_span_count = 0
        self.executor = ThreadPoolExecutor(max_workers=5)
        self.task_running = True
        self.sender_result = self.executor.submit(self.getAndSendSpan)
        self.queue = queue

    def stop(self):
        self.task_running = False
        with self.exit_cv:
            self.exit_cv.notify_all()
        self.executor.shutdown(True)
        TCLogger.info("Total send %d to pinpoint collector", self.send_span_count)

    def start(self):
        pass

    def sendSpans(self, client, spans):
        size = len(spans)
        client.sendSpans(iter(spans))
        TCLogger.debug("send %d to collector", size)

    def getAndSendSpan(self):

        def get_n_span(queue, MAX_SIZE):
            i = 0
            spans = []
            try:
                while MAX_SIZE > i:
                    spans.append(queue.get(timeout=0.05))
                    i += 1
            except Empty as e:
                pass
            return spans

        client = GrpcSpan.SpanClient(self.address, self.meta)

        while self.task_running:
            try:
                spans = get_n_span(self.queue, 10240)
                size = len(spans)
                if size == 0:
                    with self.exit_cv:
                        if not self.task_running or self.exit_cv.wait(5):
                            TCLogger.info("span send thread is done")
                            break
                        else:
                            continue
                self.send_span_count += size
                if size > 4086:  # if 5K, submit to executor
                    self.executor.submit(self.sendSpans, GrpcSpan.SpanClient(self.address, self.meta), spans)
                    TCLogger.info("submit a task to send span")
                else:
                    self.sendSpans(client, spans)
            except Exception as e:
                TCLogger.error("span channel catches an exception:%s", e)
                client.reconnect()
        TCLogger.warning("getAndSendSpan exit")
