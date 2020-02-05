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

# Created by eeliu at 10/18/19

import collections

import grpc

# copy from grpc example
from Common.Logger import TCLogger


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


class _ClientCallDetails(
    collections.namedtuple(
        '_ClientCallDetails',
        ('method', 'timeout', 'metadata', 'credentials')),
    grpc.ClientCallDetails):
    pass


class GrpcClient(object):
    def __init__(self, address, meta=None, maxPending=-1, try_reconnect=True):
        self.address = address
        self.meta = None
        self.max_pending_size = maxPending

        channel = grpc.insecure_channel(address)

        if meta is not None:
            self.meta = meta
            intercept_channel = grpc.intercept_channel(channel,
                                                       self._interceptor_add_header(meta))
            channel = intercept_channel

        self.channel = channel
        self.channel.subscribe(self._channel_state_change, try_reconnect)

    def _interceptor_add_header(self, header):
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

    def _channel_state_change(self, activity):
        if activity == grpc.ChannelConnectivity.TRANSIENT_FAILURE:
            self.channel_set_error()
        elif activity == grpc.ChannelConnectivity.READY:
            self.channel_set_ready()
        elif activity == grpc.ChannelConnectivity.IDLE:
            self.channel_set_idle()
        TCLogger.debug("channel state change %s dst:%s", activity, self.address)

    def channel_set_ready(self):
        raise NotImplemented()

    def channel_set_idle(self):
        raise NotImplemented()

    def channel_set_error(self):
        raise NotImplemented()

    def stop(self):
        self.channel.close()

    def start(self):
        raise NotImplemented()
