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
import time
import traceback
from queue import Empty

import Service_pb2_grpc
from CollectorAgent.GrpcClient import GrpcClient
from Common.Logger import TCLogger


class GrpcSpan(GrpcClient):
    def __init__(self, address, meta):
        super().__init__(address, meta)

        # self.is_ok = True
        self.span_stub = Service_pb2_grpc.SpanStub(self.channel)
        self.is_running = True
        self.is_ok = False

    def channel_set_ready(self):
        self.is_ok = True

    def channel_set_idle(self):
        self.is_ok = False

    def channel_set_error(self):
        self.is_ok = False

    def stop(self):
        super().stop()
        self.is_running = False


    def startSender(self, queue):
        spans = []

        def get_N_span(queue, N):
            i = 0
            try:
                while N > i:
                    spans.append(queue.get(timeout=0.005))
                    i += 1
            except Empty as e:
                pass
            return True if i > 0 else False

        while self.is_running:
            try:
                if not get_N_span(queue, 10240):
                    time.sleep(2)
                    continue
                self.span_stub.SendSpan(iter(spans))
            except Exception as e:
                TCLogger.error("span channel, can't work:exception:%s", e)
                traceback.print_exc()
                time.sleep(10)
            finally:
                spans.clear()