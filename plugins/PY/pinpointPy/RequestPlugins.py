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

from pinpointPy import Common
from pinpointPy import Defines
from pinpointPy import pinpoint


class RequestPlugin(Common.PinTrace):
    def __init__(self, name):
        super().__init__(name)

    @staticmethod
    def isSample(*args, **kwargs):
        return True, 0, args, kwargs

    def onBefore(self, parentId, *args, **kwargs):
        trace_id, args, kwargs = super().onBefore(parentId, *args, **kwargs)
        pinpoint.add_trace_header(
            Defines.PP_APP_NAME, pinpoint.app_name(), trace_id)
        pinpoint.add_trace_header(
            Defines.PP_APP_ID, pinpoint.app_id(), trace_id)
        pinpoint.add_context(Defines.PP_APP_NAME,
                             pinpoint.app_name(), trace_id)
        request = args[0]
        ###############################################################
        pinpoint.add_trace_header(
            Defines.PP_INTERCEPTOR_NAME, 'BaseFlaskrequest', trace_id)
        pinpoint.add_trace_header(Defines.PP_REQ_URI, request.path, trace_id)
        pinpoint.add_trace_header(
            Defines.PP_REQ_CLIENT, request.remote_addr, trace_id, trace_id)
        pinpoint.add_trace_header(
            Defines.PP_REQ_SERVER, request.host, trace_id)
        pinpoint.add_trace_header(
            Defines.PP_SERVER_TYPE, Defines.PYTHON, trace_id)
        pinpoint.add_context(Defines.PP_SERVER_TYPE, Defines.PYTHON, trace_id)

        # nginx add http
        if Defines.PP_HTTP_PINPOINT_PSPANID in request.headers:
            pinpoint.add_trace_header(
                Defines.PP_PARENT_SPAN_ID, request.headers[Defines.PP_HTTP_PINPOINT_PSPANID], trace_id)

        if Defines.PP_HTTP_PINPOINT_SPANID in request.headers:
            self.sid = request.headers[Defines.PP_HTTP_PINPOINT_SPANID]
        elif Defines.PP_HEADER_PINPOINT_SPANID in request.headers:
            self.sid = request.headers[Defines.PP_HEADER_PINPOINT_SPANID]
        else:
            self.sid = pinpoint.gen_sid()
        pinpoint.add_context(Defines.PP_SPAN_ID, self.sid, trace_id)

        if Defines.PP_HTTP_PINPOINT_TRACEID in request.headers:
            self.tid = request.headers[Defines.PP_HTTP_PINPOINT_TRACEID]
        elif Defines.PP_HEADER_PINPOINT_TRACEID in request.headers:
            self.tid = request.headers[Defines.PP_HEADER_PINPOINT_TRACEID]
        else:
            self.tid = pinpoint.gen_tid()
        pinpoint.add_context(Defines.PP_TRANSCATION_ID, self.tid, trace_id)

        if Defines.PP_HTTP_PINPOINT_PAPPNAME in request.headers:
            self.pname = request.headers[Defines.PP_HTTP_PINPOINT_PAPPNAME]
            pinpoint.add_context(Defines.PP_PARENT_NAME, self.pname, trace_id)
            pinpoint.add_trace_header(
                Defines.PP_PARENT_NAME, self.pname, trace_id)

        if Defines.PP_HTTP_PINPOINT_PAPPTYPE in request.headers:
            self.ptype = request.headers[Defines.PP_HTTP_PINPOINT_PAPPTYPE]
            pinpoint.add_context(Defines.PP_PARENT_TYPE, self.ptype, trace_id)
            pinpoint.add_trace_header(Defines.PP_PARENT_TYPE, self.ptype,
                                      trace_id)

        if Defines.PP_HTTP_PINPOINT_HOST in request.headers:
            self.Ah = request.headers[Defines.PP_HTTP_PINPOINT_HOST]
            pinpoint.add_context(Defines.PP_PARENT_HOST, self.Ah, trace_id)
            pinpoint.add_trace_header(
                Defines.PP_PARENT_HOST, self.Ah, trace_id)

        # Not nginx, no http
        if Defines.PP_HEADER_PINPOINT_PSPANID in request.headers:
            pinpoint.add_trace_header(
                Defines.PP_PARENT_SPAN_ID, request.headers[Defines.PP_HEADER_PINPOINT_PSPANID], trace_id)

        if Defines.PP_HEADER_PINPOINT_PAPPNAME in request.headers:
            self.pname = request.headers[Defines.PP_HEADER_PINPOINT_PAPPNAME]
            pinpoint.add_context(Defines.PP_PARENT_NAME, self.pname, trace_id)
            pinpoint.add_trace_header(
                Defines.PP_PARENT_NAME, self.pname, trace_id)

        if Defines.PP_HEADER_PINPOINT_PAPPTYPE in request.headers:
            self.ptype = request.headers[Defines.PP_HEADER_PINPOINT_PAPPTYPE]
            pinpoint.add_context(Defines.PP_PARENT_TYPE, self.ptype, trace_id)
            pinpoint.add_trace_header(
                Defines.PP_PARENT_TYPE, self.ptype, trace_id)

        if Defines.PP_HEADER_PINPOINT_HOST in request.headers:
            self.Ah = request.headers[Defines.PP_HEADER_PINPOINT_HOST]
            pinpoint.add_context(Defines.PP_PARENT_HOST, self.Ah, trace_id)
            pinpoint.add_trace_header(
                Defines.PP_PARENT_HOST, self.Ah, trace_id)

        if Defines.PP_NGINX_PROXY in request.headers:
            pinpoint.add_trace_header(
                Defines.PP_NGINX_PROXY, request.headers[Defines.PP_NGINX_PROXY], trace_id)

        if Defines.PP_APACHE_PROXY in request.headers:
            pinpoint.add_trace_header(
                Defines.PP_APACHE_PROXY, request.headers[Defines.PP_APACHE_PROXY], trace_id)

        pinpoint.add_context(
            Defines.PP_HEADER_PINPOINT_SAMPLED, "s1", trace_id)
        if (Defines.PP_HTTP_PINPOINT_SAMPLED in request.headers and request.headers[Defines.PP_HTTP_PINPOINT_SAMPLED] == Defines.PP_NOT_SAMPLED) or pinpoint.check_trace_limit():
            pinpoint.drop_trace(trace_id)
            pinpoint.add_context(
                Defines.PP_HEADER_PINPOINT_SAMPLED, "s0", trace_id)

        pinpoint.add_trace_header(
            Defines.PP_TRANSCATION_ID, self.tid, trace_id)
        pinpoint.add_trace_header(Defines.PP_SPAN_ID, self.sid, trace_id)
        pinpoint.add_context(Defines.PP_TRANSCATION_ID, self.tid, trace_id)
        pinpoint.add_context(Defines.PP_SPAN_ID, self.sid, trace_id)
        return trace_id, args, kwargs

    def onEnd(self, trace_id, ret):
        super().onEnd(trace_id, ret)
        return ret

    def onException(self, trace_id, e):
        pinpoint.mark_as_error(str(e), "", trace_id)
        raise e
