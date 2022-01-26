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

from .Defines import *
from .Common import PinTrace
from .Span import *
import traceback
import _pinpointPy

class RequestPlugin(PinTrace):
    def __init__(self,name):
        super().__init__(name)

    def onBefore(self,*args, **kwargs):
        super().onBefore(*args, **kwargs)
        _pinpointPy.add_clue(PP_APP_NAME,APP_NAME)
        _pinpointPy.add_clue(PP_APP_ID, APP_ID)
        _pinpointPy.set_context_key(PP_APP_NAME, APP_NAME)
        request =args[0]
        ###############################################################
        # print("------------------- call before -----------------------")
        _pinpointPy.add_clue(PP_INTERCEPTOR_NAME, 'BaseFlaskrequest')
        _pinpointPy.add_clue(PP_REQ_URI, request.path)
        _pinpointPy.add_clue(PP_REQ_CLIENT, request.remote_addr)
        _pinpointPy.add_clue(PP_REQ_SERVER, request.host)
        _pinpointPy.add_clue(PP_SERVER_TYPE, PYTHON)
        _pinpointPy.set_context_key(PP_SERVER_TYPE, PYTHON)

        # nginx add http
        if PP_HTTP_PINPOINT_PSPANID in request.headers:
            _pinpointPy.add_clue(PP_PARENT_SPAN_ID, request.headers[PP_HTTP_PINPOINT_PSPANID])
            print("PINPOINT_PSPANID:", request.headers[PP_HTTP_PINPOINT_PSPANID])

        if PP_HTTP_PINPOINT_SPANID in request.headers:
            self.sid = request.headers[PP_HTTP_PINPOINT_SPANID]
        elif PP_HEADER_PINPOINT_SPANID in request.headers:
            self.sid = request.headers[PP_HEADER_PINPOINT_SPANID]
        else:
            self.sid = generateSid()
        _pinpointPy.set_context_key(PP_SPAN_ID, self.sid)

        if PP_HTTP_PINPOINT_TRACEID in request.headers:
            self.tid = request.headers[PP_HTTP_PINPOINT_TRACEID]
        elif PP_HEADER_PINPOINT_TRACEID in request.headers:
            self.tid = request.headers[PP_HEADER_PINPOINT_TRACEID]
        else:
            self.tid = generateTid()
        _pinpointPy.set_context_key(PP_TRANSCATION_ID, self.tid)

        if PP_HTTP_PINPOINT_PAPPNAME in request.headers:
            self.pname = request.headers[PP_HTTP_PINPOINT_PAPPNAME]
            _pinpointPy.set_context_key(PP_PARENT_NAME, self.pname)
            _pinpointPy.add_clue(PP_PARENT_NAME, self.pname)

        if PP_HTTP_PINPOINT_PAPPTYPE in request.headers:
            self.ptype = request.headers[PP_HTTP_PINPOINT_PAPPTYPE]
            _pinpointPy.set_context_key(PP_PARENT_TYPE, self.ptype)
            _pinpointPy.add_clue(PP_PARENT_TYPE, self.ptype)

        if PP_HTTP_PINPOINT_HOST in request.headers:
            self.Ah = request.headers[PP_HTTP_PINPOINT_HOST]
            _pinpointPy.set_context_key(PP_PARENT_HOST, self.Ah)
            _pinpointPy.add_clue(PP_PARENT_HOST, self.Ah)

        # Not nginx, no http
        if PP_HEADER_PINPOINT_PSPANID in request.headers:
            _pinpointPy.add_clue(PP_PARENT_SPAN_ID, request.headers[PP_HEADER_PINPOINT_PSPANID])
            # print("PINPOINT_PSPANID:", request.headers[PP_HEADER_PINPOINT_PSPANID])

        if PP_HEADER_PINPOINT_PAPPNAME in request.headers:
            self.pname = request.headers[PP_HEADER_PINPOINT_PAPPNAME]
            _pinpointPy.set_context_key(PP_PARENT_NAME, self.pname)
            _pinpointPy.add_clue(PP_PARENT_NAME, self.pname)

        if PP_HEADER_PINPOINT_PAPPTYPE in request.headers:
            self.ptype = request.headers[PP_HEADER_PINPOINT_PAPPTYPE]
            _pinpointPy.set_context_key(PP_PARENT_TYPE, self.ptype)
            _pinpointPy.add_clue(PP_PARENT_TYPE, self.ptype)

        if PP_HEADER_PINPOINT_HOST in request.headers:
            self.Ah = request.headers[PP_HEADER_PINPOINT_HOST]
            _pinpointPy.set_context_key(PP_PARENT_HOST, self.Ah)
            _pinpointPy.add_clue(PP_PARENT_HOST, self.Ah)

        if PP_NGINX_PROXY in request.headers:
            _pinpointPy.add_clue(PP_NGINX_PROXY, request.headers[PP_NGINX_PROXY])

        if PP_APACHE_PROXY in request.headers:
            _pinpointPy.add_clue(PP_APACHE_PROXY, request.headers[PP_APACHE_PROXY])

        _pinpointPy.set_context_key(PP_HEADER_PINPOINT_SAMPLED, "s1")
        if (PP_HTTP_PINPOINT_SAMPLED in request.headers and request.headers[PP_HTTP_PINPOINT_SAMPLED] == PP_NOT_SAMPLED) or _pinpointPy.check_tracelimit():
            _pinpointPy.drop_trace()
            _pinpointPy.set_context_key(PP_HEADER_PINPOINT_SAMPLED, "s0")

        _pinpointPy.add_clue(PP_TRANSCATION_ID, self.tid)
        _pinpointPy.add_clue(PP_SPAN_ID, self.sid)
        _pinpointPy.set_context_key(PP_TRANSCATION_ID, self.tid)
        _pinpointPy.set_context_key(PP_SPAN_ID, self.sid)
        return args, kwargs

    def onEnd(self,ret):
        super().onEnd(ret)
        return ret

    def onException(self, e):
        _pinpointPy.mark_as_error(str(e),"",0)
        raise e



