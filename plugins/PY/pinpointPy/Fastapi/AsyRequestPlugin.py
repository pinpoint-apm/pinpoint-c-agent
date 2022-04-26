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


from .AsyCommon import AsynPinTrace
from .. import pinpoint
from .. import Defines


class AsyRequestPlugin(AsynPinTrace):
    def __init__(self, name):
        super().__init__(name)

    def onBefore(self, *args, **kwargs):
        super().onBefore(*args, **kwargs)
        pinpoint.add_trace_header(Defines.PP_APP_NAME, pinpoint.app_name(), self.traceId)
        pinpoint.add_trace_header(Defines.PP_APP_ID, pinpoint.app_id(), self.traceId)
        pinpoint.add_context(Defines.PP_APP_NAME, pinpoint.app_name(), self.traceId)
        ###############################################################
        request = args[0]
        # pinpoint.add_trace_header(Defines.PP_INTERCEPTOR_NAME, 'fastapi.RequestHandler',self.traceId)
        # pinpoint.add_trace_header(Defines.PP_REQ_URI, request.uri, self.traceId)
        # pinpoint.add_trace_header(Defines.PP_REQ_CLIENT, request.remote_ip, self.traceId)
        # pinpoint.add_trace_header(Defines.PP_REQ_SERVER, request.host_name, self.traceId)
        pinpoint.add_trace_header(Defines.PP_SERVER_TYPE, Defines.PYTHON, self.traceId)
        pinpoint.add_context(Defines.PP_SERVER_TYPE, Defines.PYTHON, self.traceId)

        # nginx add http
        if Defines.PP_HTTP_PINPOINT_PSPANID in request.headers:
            pinpoint.add_trace_header(Defines.PP_PARENT_SPAN_ID, request.headers[Defines.PP_HTTP_PINPOINT_PSPANID], self.traceId)
        if Defines.PP_HTTP_PINPOINT_SPANID in request.headers:
            self.sid = request.headers[Defines.PP_HTTP_PINPOINT_SPANID]
        elif Defines.PP_HEADER_PINPOINT_SPANID in request.headers:
            self.sid = request.headers[Defines.PP_HEADER_PINPOINT_SPANID]
        else:
            self.sid = pinpoint.gen_sid()
        pinpoint.add_context(Defines.PP_SPAN_ID, self.sid, self.traceId)

        if Defines.PP_HTTP_PINPOINT_TRACEID in request.headers:
            self.tid = request.headers[Defines.PP_HTTP_PINPOINT_TRACEID]
        elif Defines.PP_HEADER_PINPOINT_TRACEID in request.headers:
            self.tid = request.headers[Defines.PP_HEADER_PINPOINT_TRACEID]
        else:
            self.tid = pinpoint.gen_tid()
        pinpoint.add_context(Defines.PP_TRANSCATION_ID, self.tid, self.traceId)

        if Defines.PP_HTTP_PINPOINT_PAPPNAME in request.headers:
            self.pname = request.headers[Defines.PP_HTTP_PINPOINT_PAPPNAME]
            pinpoint.add_context(Defines.PP_PARENT_NAME, self.pname, self.traceId)
            pinpoint.add_trace_header(Defines.PP_PARENT_NAME, self.pname, self.traceId)

        if Defines.PP_HTTP_PINPOINT_PAPPTYPE in request.headers:
            self.ptype = request.headers[Defines.PP_HTTP_PINPOINT_PAPPTYPE]
            pinpoint.add_context(Defines.PP_PARENT_TYPE, self.ptype, self.traceId)
            pinpoint.add_trace_header(Defines.PP_PARENT_TYPE, self.ptype, self.traceId)

        if Defines.PP_HTTP_PINPOINT_HOST in request.headers:
            self.Ah = request.headers[Defines.PP_HTTP_PINPOINT_HOST]
            pinpoint.add_context(Defines.PP_PARENT_HOST, self.Ah, self.traceId)
            pinpoint.add_trace_header(Defines.PP_PARENT_HOST, self.Ah, self.traceId)

        # Not nginx, no http
        if Defines.PP_HEADER_PINPOINT_PSPANID in request.headers:
            pinpoint.add_trace_header(Defines.PP_PARENT_SPAN_ID, request.headers[Defines.PP_HEADER_PINPOINT_PSPANID], self.traceId)

        if Defines.PP_HEADER_PINPOINT_PAPPNAME in request.headers:
            self.pname = request.headers[Defines.PP_HEADER_PINPOINT_PAPPNAME]
            pinpoint.add_context(Defines.PP_PARENT_NAME, self.pname, self.traceId)
            pinpoint.add_trace_header(Defines.PP_PARENT_NAME, self.pname, self.traceId)

        if Defines.PP_HEADER_PINPOINT_PAPPTYPE in request.headers:
            self.ptype = request.headers[Defines.PP_HEADER_PINPOINT_PAPPTYPE]
            pinpoint.add_context(Defines.PP_PARENT_TYPE, self.ptype, self.traceId)
            pinpoint.add_trace_header(Defines.PP_PARENT_TYPE, self.ptype, self.traceId)

        if Defines.PP_HEADER_PINPOINT_HOST in request.headers:
            self.Ah = request.headers[Defines.PP_HEADER_PINPOINT_HOST]
            pinpoint.add_context(Defines.PP_PARENT_HOST, self.Ah, self.traceId)
            pinpoint.add_trace_header(Defines.PP_PARENT_HOST, self.Ah, self.traceId)

        if Defines.PP_NGINX_PROXY in request.headers:
            pinpoint.add_trace_header(Defines.PP_NGINX_PROXY, request.headers[Defines.PP_NGINX_PROXY], self.traceId)

        if Defines.PP_APACHE_PROXY in request.headers:
            pinpoint.add_trace_header(Defines.PP_APACHE_PROXY, request.headers[Defines.PP_APACHE_PROXY], self.traceId)

        pinpoint.add_context("Pinpoint-Sampled", "s1", self.traceId)
        if (Defines.PP_HTTP_PINPOINT_SAMPLED in request.headers and request.headers[Defines.PP_HTTP_PINPOINT_SAMPLED] == Defines.PP_NOT_SAMPLED) or (Defines.PP_HEADER_PINPOINT_SAMPLED in request.headers and request.headers[Defines.PP_HEADER_PINPOINT_SAMPLED] == Defines.PP_NOT_SAMPLED) or pinpoint.check_trace_limit():
            pinpoint.drop_trace(self.traceId)
            pinpoint.add_context("Pinpoint-Sampled", "s0", self.traceId)

        pinpoint.add_trace_header(Defines.PP_TRANSCATION_ID, self.tid, self.traceId)
        pinpoint.add_trace_header(Defines.PP_SPAN_ID, self.sid, self.traceId)
        pinpoint.add_context(Defines.PP_TRANSCATION_ID, self.tid, self.traceId)
        pinpoint.add_context(Defines.PP_SPAN_ID, self.sid, self.traceId)
        return args, kwargs

    def onEnd(self, ret):
        super().onEnd(ret)
        self.isLimit = False
        return ret

    def onException(self, e):
        pinpoint.mark_as_error(str(e), "", 0, self.traceId)
        raise e
