#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 10/31/19
import Service_pb2_grpc
from CollectorAgent.GrpcClient import GrpcClient
from Common.Logger import TCLogger


class SpanClient(GrpcClient):
    def __init__(self,span_callback_fn,address,meta=None,maxPending=-1):
        super().__init__(address, meta, maxPending)

        self.is_ok = False
        assert (callable(span_callback_fn))
        self.span_callback_fn = span_callback_fn
        self.span_stub = Service_pb2_grpc.SpanStub(self.channel)
        self._start_span()


    def channel_is_ready(self):
        self.is_ok = True

    def channel_is_idle(self):
        self.is_ok = True

    def channel_is_error(self):
        self.is_ok = False

    def _span_response(self,*args):
        print(args)

    def _get_span(self):
        #todo add is_ok checking
        while True:
            span = self.span_callback_fn()
            if span is not None:
                yield span
            else:
                TCLogger.error(" get span failed, checking the span_callback_fn")

    def _start_span(self):
        self.future = self.span_stub.SendSpan.future(self._get_span())
        self.future.add_done_callback(self._span_response)