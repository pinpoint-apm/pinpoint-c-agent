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

from . import Common
from . import Defines
from . import pinpoint

class RequestPlugin(Common.PinTrace):
    def __init__(self,name):
        super().__init__(name)

    def onBefore(self,*args, **kwargs):
        super().onBefore(*args, **kwargs)
        pinpoint.add_trace_header(Defines.PP_APP_NAME, pinpoint.app_name())
        pinpoint.add_trace_header(Defines.PP_APP_ID, pinpoint.app_id())
        pinpoint.add_context(Defines.PP_APP_NAME, pinpoint.app_name())
        request =args[0]
        ###############################################################
        # print("------------------- call before -----------------------")
        pinpoint.add_trace_header(Defines.PP_INTERCEPTOR_NAME, 'BaseFlaskrequest')
        pinpoint.add_trace_header(Defines.PP_REQ_URI, request.path)
        pinpoint.add_trace_header(Defines.PP_REQ_CLIENT, request.remote_addr)
        pinpoint.add_trace_header(Defines.PP_REQ_SERVER, request.host)
        pinpoint.add_trace_header(Defines.PP_SERVER_TYPE, Defines.PYTHON)
        pinpoint.add_context(Defines.PP_SERVER_TYPE, Defines.PYTHON)

        # nginx add http
        if Defines.PP_HTTP_PINPOINT_PSPANID in request.headers:
            pinpoint.add_trace_header(Defines.PP_PARENT_SPAN_ID, request.headers[Defines.PP_HTTP_PINPOINT_PSPANID])
            print("PINPOINT_PSPANID:", request.headers[Defines.PP_HTTP_PINPOINT_PSPANID])

        if Defines.PP_HTTP_PINPOINT_SPANID in request.headers:
            self.sid = request.headers[Defines.PP_HTTP_PINPOINT_SPANID]
        elif Defines.PP_HEADER_PINPOINT_SPANID in request.headers:
            self.sid = request.headers[Defines.PP_HEADER_PINPOINT_SPANID]
        else:
            self.sid = pinpoint.gen_sid()
        pinpoint.add_context(Defines.PP_SPAN_ID, self.sid)

        if Defines.PP_HTTP_PINPOINT_TRACEID in request.headers:
            self.tid = request.headers[Defines.PP_HTTP_PINPOINT_TRACEID]
        elif Defines.PP_HEADER_PINPOINT_TRACEID in request.headers:
            self.tid = request.headers[Defines.PP_HEADER_PINPOINT_TRACEID]
        else:
            self.tid = pinpoint.gen_tid()
        pinpoint.add_context(Defines.PP_TRANSCATION_ID, self.tid)

        if Defines.PP_HTTP_PINPOINT_PAPPNAME in request.headers:
            self.pname = request.headers[Defines.PP_HTTP_PINPOINT_PAPPNAME]
            pinpoint.add_context(Defines.PP_PARENT_NAME, self.pname)
            pinpoint.add_trace_header(Defines.PP_PARENT_NAME, self.pname)

        if Defines.PP_HTTP_PINPOINT_PAPPTYPE in request.headers:
            self.ptype = request.headers[Defines.PP_HTTP_PINPOINT_PAPPTYPE]
            pinpoint.add_context(Defines.PP_PARENT_TYPE, self.ptype)
            pinpoint.add_trace_header(Defines.PP_PARENT_TYPE, self.ptype)

        if Defines.PP_HTTP_PINPOINT_HOST in request.headers:
            self.Ah = request.headers[Defines.PP_HTTP_PINPOINT_HOST]
            pinpoint.add_context(Defines.PP_PARENT_HOST, self.Ah)
            pinpoint.add_trace_header(Defines.PP_PARENT_HOST, self.Ah)

        # Not nginx, no http
        if Defines.PP_HEADER_PINPOINT_PSPANID in request.headers:
            pinpoint.add_trace_header(Defines.PP_PARENT_SPAN_ID, request.headers[Defines.PP_HEADER_PINPOINT_PSPANID])
            # print("PINPOINT_PSPANID:", request.headers[PP_HEADER_PINPOINT_PSPANID])

        if Defines.PP_HEADER_PINPOINT_PAPPNAME in request.headers:
            self.pname = request.headers[Defines.PP_HEADER_PINPOINT_PAPPNAME]
            pinpoint.add_context(Defines.PP_PARENT_NAME, self.pname)
            pinpoint.add_trace_header(Defines.PP_PARENT_NAME, self.pname)

        if Defines.PP_HEADER_PINPOINT_PAPPTYPE in request.headers:
            self.ptype = request.headers[Defines.PP_HEADER_PINPOINT_PAPPTYPE]
            pinpoint.add_context(Defines.PP_PARENT_TYPE, self.ptype)
            pinpoint.add_trace_header(Defines.PP_PARENT_TYPE, self.ptype)

        if Defines.PP_HEADER_PINPOINT_HOST in request.headers:
            self.Ah = request.headers[Defines.PP_HEADER_PINPOINT_HOST]
            pinpoint.add_context(Defines.PP_PARENT_HOST, self.Ah)
            pinpoint.add_trace_header(Defines.PP_PARENT_HOST, self.Ah)

        if Defines.PP_NGINX_PROXY in request.headers:
            pinpoint.add_trace_header(Defines.PP_NGINX_PROXY, request.headers[Defines.PP_NGINX_PROXY])

        if Defines.PP_APACHE_PROXY in request.headers:
            pinpoint.add_trace_header(Defines.PP_APACHE_PROXY, request.headers[Defines.PP_APACHE_PROXY])

        pinpoint.add_context(Defines.PP_HEADER_PINPOINT_SAMPLED, "s1")
        if (Defines.PP_HTTP_PINPOINT_SAMPLED in request.headers and request.headers[Defines.PP_HTTP_PINPOINT_SAMPLED] == Defines.PP_NOT_SAMPLED) or pinpoint.check_trace_limit():
            pinpoint.drop_trace()
            pinpoint.add_context(Defines.PP_HEADER_PINPOINT_SAMPLED, "s0")

        pinpoint.add_trace_header(Defines.PP_TRANSCATION_ID, self.tid)
        pinpoint.add_trace_header(Defines.PP_SPAN_ID, self.sid)
        pinpoint.add_context(Defines.PP_TRANSCATION_ID, self.tid)
        pinpoint.add_context(Defines.PP_SPAN_ID, self.sid)
        return args, kwargs

    def onEnd(self,ret):
        super().onEnd(ret)
        return ret

    def onException(self, e):
        pinpoint.mark_as_error(str(e), "", 0)
        raise e



