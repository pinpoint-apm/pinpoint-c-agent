#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 10/18/19

import grpc

from CollectorAgent.GrpcAgentImplement import NOT_READY,READY,BUSY
from Proto.grpc import Service_pb2_grpc
from Proto.grpc.Stat_pb2 import PAgentInfo, PPing
from Span_pb2 import PSqlMetaData, PApiMetaData, PSpanMessage, PSpan, PTransactionId

# copy from grpc example
class _GenericClientInterceptor(
        grpc.UnaryUnaryClientInterceptor, grpc.UnaryStreamClientInterceptor,
        grpc.StreamUnaryClientInterceptor, grpc.StreamStreamClientInterceptor):

    def __init__(self, interceptor_function):
        self._fn = interceptor_function

    def intercept_unary_unary(self, continuation, client_call_details, request):
        new_details, new_request_iterator, postprocess = self._fn(
            client_call_details, iter((request,)), False, False)
        response = continuation(new_details, next(new_request_iterator))
        return postprocess(response) if postprocess else response

    def intercept_unary_stream(self, continuation, client_call_details,
                               request):
        new_details, new_request_iterator, postprocess = self._fn(
            client_call_details, iter((request,)), False, True)
        response_it = continuation(new_details, next(new_request_iterator))
        return postprocess(response_it) if postprocess else response_it

    def intercept_stream_unary(self, continuation, client_call_details,
                               request_iterator):
        new_details, new_request_iterator, postprocess = self._fn(
            client_call_details, request_iterator, True, False)
        response = continuation(new_details, new_request_iterator)
        return postprocess(response) if postprocess else response

    def intercept_stream_stream(self, continuation, client_call_details,
                                request_iterator):
        new_details, new_request_iterator, postprocess = self._fn(
            client_call_details, request_iterator, True, True)
        response_it = continuation(new_details, new_request_iterator)
        return postprocess(response_it) if postprocess else response_it


class GrpcClient(object):
    def __init__(self,address,meta=None,maxPending=-1):
        self.address = address
        self.meta = meta
        self.max_pending_size = maxPending
        self.state = NOT_READY
        channel = grpc.insecure_channel(address)
        if meta is not None:
            pass
        if maxPending != -1:
            pass
        self.channel =channel
        self.channel.subscribe(self.channel_state_change)


    def _add_meta_header(self):
        pass

    def get_stub(self):
        pass

    def get_state(self):
        return self.state

    def _interceptor_add_header(self,header):
        def intercept_call(client_call_details, request_iterator, request_streaming,
                           response_streaming):
            metadata = []
            if client_call_details.metadata is not None:
                metadata = list(client_call_details.metadata)
            metadata += header
            client_call_details = _ClientCallDetails(
                client_call_details.method, client_call_details.timeout, metadata,
                client_call_details.credentials)
            return client_call_details, request_iterator, None
        return _GenericClientInterceptor(intercept_call)

    def channel_state_change(self,activity):
        if activity == grpc.ChannelConnectivity.TRANSIENT_FAILURE:
            self.state = NOT_READY
        elif activity == grpc.ChannelConnectivity.CONNECTING:
            self.state = NOT_READY
        elif activity ==  grpc.ChannelConnectivity.READY:
            self.state = READY

class AgentClient(GrpcClient):
    def __init__(self,address,meta=None,maxPending=-1):
        super().__init__(address,meta,maxPending)