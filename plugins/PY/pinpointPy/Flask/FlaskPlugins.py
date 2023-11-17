#!/usr/bin/env python
# -*- coding: UTF-8 -*-

# ------------------------------------------------------------------------------
#  Copyright  2020. NAVER Corp.                                                -
#                                                                              -
#  Licensed under the Apache License, Version 2.0 (the "License");             -
#  you may not use this file except in compliance with the License.            -
#  You may obtain a copy of the License at                                     -
#                                                                              -
#   http://www.apache.org/licenses/LICENSE-2.0                                 -
#                                                                              -
#  Unless required by applicable law or agreed to in writing, software         -
#  distributed under the License is distributed on an "AS IS" BASIS,           -
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.    -
#  See the License for the specific language governing permissions and         -
#  limitations under the License.                                              -
# ------------------------------------------------------------------------------

# Created by eeliu at 3/5/20

from flask import Request

from pinpointPy import Common, pinpoint, Defines


class BaseFlaskPlugins(Common.PinTrace):
    def __init__(self, name):
        super().__init__(name)

    @staticmethod
    def isSample(*args, **kwargs):
        return True, 0, args, kwargs

    def onBefore(self, parentId, *args, **kwargs):
        traceId, args, kwargs = super().onBefore(parentId, *args, **kwargs)
        request = Request(args[1])
        pinpoint.add_trace_header(
            Defines.PP_APP_NAME, pinpoint.app_name(), traceId)
        pinpoint.add_context(Defines.PP_APP_NAME, pinpoint.app_name(), traceId)
        pinpoint.add_trace_header(
            Defines.PP_APP_ID, pinpoint.app_id(), traceId)
        ###############################################################
        pinpoint.add_trace_header(
            Defines.PP_INTERCEPTOR_NAME, 'BaseFlaskrequest', traceId)
        pinpoint.add_trace_header(Defines.PP_REQ_URI, request.path, traceId)
        if request.remote_addr:
            pinpoint.add_trace_header(
                Defines.PP_REQ_CLIENT, request.remote_addr, traceId)

        pinpoint.add_trace_header(Defines.PP_REQ_SERVER, request.host, traceId)
        pinpoint.add_trace_header(
            Defines.PP_SERVER_TYPE, Defines.PYTHON, traceId)
        pinpoint.add_context(Defines.PP_SERVER_TYPE, Defines.PYTHON, traceId)
        # nginx add http
        if Defines.PP_HTTP_PINPOINT_PSPANID in request.headers:
            pinpoint.add_trace_header(
                Defines.PP_PARENT_SPAN_ID, request.headers[Defines.PP_HTTP_PINPOINT_PSPANID], traceId)

        if Defines.PP_HTTP_PINPOINT_SPANID in request.headers:
            self.sid = request.headers[Defines.PP_HTTP_PINPOINT_SPANID]
        elif Defines.PP_HEADER_PINPOINT_SPANID in request.headers:
            self.sid = request.headers[Defines.PP_HEADER_PINPOINT_SPANID]
        else:
            self.sid = pinpoint.gen_sid()

        if Defines.PP_HTTP_PINPOINT_TRACEID in request.headers:
            self.tid = request.headers[Defines.PP_HTTP_PINPOINT_TRACEID]
        elif Defines.PP_HEADER_PINPOINT_TRACEID in request.headers:
            self.tid = request.headers[Defines.PP_HEADER_PINPOINT_TRACEID]
        else:
            self.tid = pinpoint.gen_tid()

        if Defines.PP_HTTP_PINPOINT_PAPPNAME in request.headers:
            self.pname = request.headers[Defines.PP_HTTP_PINPOINT_PAPPNAME]
            pinpoint.add_context(Defines.PP_PARENT_NAME, self.pname, traceId)
            pinpoint.add_trace_header(
                Defines.PP_PARENT_NAME, self.pname, traceId)

        if Defines.PP_HTTP_PINPOINT_PAPPTYPE in request.headers:
            self.ptype = request.headers[Defines.PP_HTTP_PINPOINT_PAPPTYPE]
            pinpoint.add_context(Defines.PP_PARENT_TYPE, self.ptype, traceId)
            pinpoint.add_trace_header(
                Defines.PP_PARENT_TYPE, self.ptype, traceId)

        if Defines.PP_HTTP_PINPOINT_HOST in request.headers:
            self.Ah = request.headers[Defines.PP_HTTP_PINPOINT_HOST]
            pinpoint.add_context(Defines.PP_PARENT_HOST, self.Ah, traceId)
            pinpoint.add_trace_header(Defines.PP_PARENT_HOST, self.Ah, traceId)

        # Not nginx, no http
        if Defines.PP_HEADER_PINPOINT_PSPANID in request.headers:
            pinpoint.add_trace_header(
                Defines.PP_PARENT_SPAN_ID, request.headers[Defines.PP_HEADER_PINPOINT_PSPANID], traceId)
            # print("PINPOINT_PSPANID:", request.headers[PP_HEADER_PINPOINT_PSPANID])

        if Defines.PP_HEADER_PINPOINT_PAPPNAME in request.headers:
            self.pname = request.headers[Defines.PP_HEADER_PINPOINT_PAPPNAME]
            pinpoint.add_context(Defines.PP_PARENT_NAME, self.pname, traceId)
            pinpoint.add_trace_header(
                Defines.PP_PARENT_NAME, self.pname, traceId)

        if Defines.PP_HEADER_PINPOINT_PAPPTYPE in request.headers:
            self.ptype = request.headers[Defines.PP_HEADER_PINPOINT_PAPPTYPE]
            pinpoint.add_context(Defines.PP_PARENT_TYPE, self.ptype, traceId)
            pinpoint.add_trace_header(
                Defines.PP_PARENT_TYPE, self.ptype, traceId)

        if Defines.PP_HEADER_PINPOINT_HOST in request.headers:
            self.Ah = request.headers[Defines.PP_HEADER_PINPOINT_HOST]
            pinpoint.add_context(Defines.PP_PARENT_HOST, self.Ah, traceId)
            pinpoint.add_trace_header(Defines.PP_PARENT_HOST, self.Ah, traceId)

        if Defines.PP_NGINX_PROXY in request.headers:
            pinpoint.add_trace_header(
                Defines.PP_NGINX_PROXY, request.headers[Defines.PP_NGINX_PROXY], traceId)

        if Defines.PP_APACHE_PROXY in request.headers:
            pinpoint.add_trace_header(
                Defines.PP_APACHE_PROXY, request.headers[Defines.PP_APACHE_PROXY], traceId)

        pinpoint.add_context(Defines.PP_HEADER_PINPOINT_SAMPLED, "s1", traceId)
        if (Defines.PP_HTTP_PINPOINT_SAMPLED in request.headers and request.headers[Defines.PP_HTTP_PINPOINT_SAMPLED] == Defines.PP_NOT_SAMPLED) or (Defines.PP_HEADER_PINPOINT_SAMPLED in request.headers and request.headers[Defines.PP_HEADER_PINPOINT_SAMPLED] == Defines.PP_NOT_SAMPLED) or pinpoint.check_trace_limit():
            pinpoint.drop_trace(traceId)
            pinpoint.add_context(
                Defines.PP_HEADER_PINPOINT_SAMPLED, "s0", traceId)

        pinpoint.add_trace_header(Defines.PP_TRANSCATION_ID, self.tid, traceId)
        pinpoint.add_context(Defines.PP_TRANSCATION_ID, self.tid, traceId)
        pinpoint.add_trace_header(Defines.PP_SPAN_ID, self.sid, traceId)
        pinpoint.add_context(Defines.PP_SPAN_ID, self.sid, traceId)
        pinpoint.add_trace_header_v2(
            Defines.PP_HTTP_METHOD, request.method, traceId)
        ###############################################################
        return traceId, args, kwargs

    def onEnd(self, traceId, ret):
        super().onEnd(traceId, ret)
        return ret

    def onException(self, e):
        pinpoint.mark_as_error(str(e), "", 0)
        raise e
