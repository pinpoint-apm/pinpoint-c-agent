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


from pinpointPy.Fastapi.AsyCommon import AsyncPinTrace
from pinpointPy import pinpoint
from pinpointPy import Defines


class AsyRequestPlugin(AsyncPinTrace):
    def __init__(self, name):
        super().__init__(name)

    def onBefore(self, parentId, *args, **kwargs):
        traceId, args, kwargs = super().onBefore(parentId, *args, **kwargs)
        pinpoint.add_trace_header(
            Defines.PP_APP_NAME, pinpoint.app_name(), traceId)
        pinpoint.add_trace_header(
            Defines.PP_APP_ID, pinpoint.app_id(), traceId)
        pinpoint.add_context(Defines.PP_APP_NAME, pinpoint.app_name(), traceId)
        ###############################################################
        request = args[0]

        pinpoint.add_trace_header(
            Defines.PP_SERVER_TYPE, Defines.PYTHON, traceId)
        pinpoint.add_context(Defines.PP_SERVER_TYPE, Defines.PYTHON, traceId)

        # nginx add http
        if Defines.PP_HTTP_PINPOINT_PSPANID in request.headers:
            pinpoint.add_trace_header(
                Defines.PP_PARENT_SPAN_ID, request.headers[Defines.PP_HTTP_PINPOINT_PSPANID], traceId)
        if Defines.PP_HTTP_PINPOINT_SPANID in request.headers:
            sid = request.headers[Defines.PP_HTTP_PINPOINT_SPANID]
        elif Defines.PP_HEADER_PINPOINT_SPANID in request.headers:
            sid = request.headers[Defines.PP_HEADER_PINPOINT_SPANID]
        else:
            sid = pinpoint.gen_sid()

        if Defines.PP_HTTP_PINPOINT_TRACEID in request.headers:
            tid = request.headers[Defines.PP_HTTP_PINPOINT_TRACEID]
        elif Defines.PP_HEADER_PINPOINT_TRACEID in request.headers:
            tid = request.headers[Defines.PP_HEADER_PINPOINT_TRACEID]
        else:
            tid = pinpoint.gen_tid()

        if Defines.PP_HTTP_PINPOINT_PAPPNAME in request.headers:
            pname = request.headers[Defines.PP_HTTP_PINPOINT_PAPPNAME]
            pinpoint.add_context(Defines.PP_PARENT_NAME, pname, traceId)
            pinpoint.add_trace_header(Defines.PP_PARENT_NAME, pname, traceId)

        if Defines.PP_HTTP_PINPOINT_PAPPTYPE in request.headers:
            ptype = request.headers[Defines.PP_HTTP_PINPOINT_PAPPTYPE]
            pinpoint.add_context(Defines.PP_PARENT_TYPE, ptype, traceId)
            pinpoint.add_trace_header(Defines.PP_PARENT_TYPE, ptype, traceId)

        if Defines.PP_HTTP_PINPOINT_HOST in request.headers:
            Ah = request.headers[Defines.PP_HTTP_PINPOINT_HOST]
            pinpoint.add_context(Defines.PP_PARENT_HOST, Ah, traceId)
            pinpoint.add_trace_header(Defines.PP_PARENT_HOST, Ah, traceId)

        # Not nginx, no http
        if Defines.PP_HEADER_PINPOINT_PSPANID in request.headers:
            pinpoint.add_trace_header(
                Defines.PP_PARENT_SPAN_ID, request.headers[Defines.PP_HEADER_PINPOINT_PSPANID], traceId)

        if Defines.PP_HEADER_PINPOINT_PAPPNAME in request.headers:
            pname = request.headers[Defines.PP_HEADER_PINPOINT_PAPPNAME]
            pinpoint.add_context(Defines.PP_PARENT_NAME, pname, traceId)
            pinpoint.add_trace_header(Defines.PP_PARENT_NAME, pname, traceId)

        if Defines.PP_HEADER_PINPOINT_PAPPTYPE in request.headers:
            ptype = request.headers[Defines.PP_HEADER_PINPOINT_PAPPTYPE]
            pinpoint.add_context(Defines.PP_PARENT_TYPE, ptype, traceId)
            pinpoint.add_trace_header(Defines.PP_PARENT_TYPE, ptype, traceId)

        if Defines.PP_HEADER_PINPOINT_HOST in request.headers:
            Ah = request.headers[Defines.PP_HEADER_PINPOINT_HOST]
            pinpoint.add_context(Defines.PP_PARENT_HOST, Ah, traceId)
            pinpoint.add_trace_header(Defines.PP_PARENT_HOST, Ah, traceId)

        if Defines.PP_NGINX_PROXY in request.headers:
            pinpoint.add_trace_header(
                Defines.PP_NGINX_PROXY, request.headers[Defines.PP_NGINX_PROXY], traceId)

        if Defines.PP_APACHE_PROXY in request.headers:
            pinpoint.add_trace_header(
                Defines.PP_APACHE_PROXY, request.headers[Defines.PP_APACHE_PROXY], traceId)

        pinpoint.add_context("Pinpoint-Sampled", "s1", traceId)
        if (Defines.PP_HTTP_PINPOINT_SAMPLED in request.headers and request.headers[Defines.PP_HTTP_PINPOINT_SAMPLED] == Defines.PP_NOT_SAMPLED) or (Defines.PP_HEADER_PINPOINT_SAMPLED in request.headers and request.headers[Defines.PP_HEADER_PINPOINT_SAMPLED] == Defines.PP_NOT_SAMPLED) or pinpoint.check_trace_limit():
            pinpoint.drop_trace(traceId)
            pinpoint.add_context("Pinpoint-Sampled", "s0", traceId)

        pinpoint.add_trace_header(Defines.PP_TRANSCATION_ID, tid, traceId)
        pinpoint.add_context(Defines.PP_TRANSCATION_ID, tid, traceId)
        pinpoint.add_trace_header(Defines.PP_SPAN_ID, sid, traceId)
        pinpoint.add_context(Defines.PP_SPAN_ID, sid, traceId)
        return traceId, args, kwargs

    def onEnd(self, traceId, ret):
        super().onEnd(traceId, ret)
        return ret

    def onException(self, traceId, e):
        pinpoint.mark_as_error(str(e), "", 0, traceId)
        raise e
