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
from common import *
import _pinpointPy


class AsyRequestPlugin(AsynPinTrace):
    def __init__(self, name):
        super().__init__(name)

    def onBefore(self, *args, **kwargs):
        super().onBefore(*args, **kwargs)
        _pinpointPy.add_clue(PP_APP_NAME, APP_NAME, self.traceId)
        _pinpointPy.add_clue(PP_APP_ID, APP_ID, self.traceId)
        _pinpointPy.set_context_key(PP_APP_NAME, APP_NAME, self.traceId)
        ###############################################################
        request = args[0]
        # _pinpointPy.add_clue(PP_INTERCEPTOR_NAME, 'tornado.web.RequestHandler',self.node_id)
        # _pinpointPy.add_clue(PP_REQ_URI, request.uri, self.traceId)
        # _pinpointPy.add_clue(PP_REQ_CLIENT, request.remote_ip, self.traceId)
        # _pinpointPy.add_clue(PP_REQ_SERVER, request.host_name, self.traceId)
        _pinpointPy.add_clue(PP_SERVER_TYPE, PYTHON, self.traceId)
        _pinpointPy.set_context_key(PP_SERVER_TYPE, PYTHON, self.traceId)

        # nginx add http
        if PP_HTTP_PINPOINT_PSPANID in request.headers:
            _pinpointPy.add_clue(PP_PARENT_SPAN_ID, request.headers[PP_HTTP_PINPOINT_PSPANID], self.traceId)
        if PP_HTTP_PINPOINT_SPANID in request.headers:
            self.sid = request.headers[PP_HTTP_PINPOINT_SPANID]
        elif PP_HEADER_PINPOINT_SPANID in request.headers:
            self.sid = request.headers[PP_HEADER_PINPOINT_SPANID]
        else:
            self.sid = generateSid()
        _pinpointPy.set_context_key(PP_SPAN_ID, self.sid, self.traceId)

        if PP_HTTP_PINPOINT_TRACEID in request.headers:
            self.tid = request.headers[PP_HTTP_PINPOINT_TRACEID]
        elif PP_HEADER_PINPOINT_TRACEID in request.headers:
            self.tid = request.headers[PP_HEADER_PINPOINT_TRACEID]
        else:
            self.tid = generateTid()
        _pinpointPy.set_context_key(PP_TRANSCATION_ID, self.tid, self.traceId)

        if PP_HTTP_PINPOINT_PAPPNAME in request.headers:
            self.pname = request.headers[PP_HTTP_PINPOINT_PAPPNAME]
            _pinpointPy.set_context_key(PP_PARENT_NAME, self.pname, self.traceId)
            _pinpointPy.add_clue(PP_PARENT_NAME, self.pname, self.traceId)

        if PP_HTTP_PINPOINT_PAPPTYPE in request.headers:
            self.ptype = request.headers[PP_HTTP_PINPOINT_PAPPTYPE]
            _pinpointPy.set_context_key(PP_PARENT_TYPE, self.ptype, self.traceId)
            _pinpointPy.add_clue(PP_PARENT_TYPE, self.ptype, self.traceId)

        if PP_HTTP_PINPOINT_HOST in request.headers:
            self.Ah = request.headers[PP_HTTP_PINPOINT_HOST]
            _pinpointPy.set_context_key(PP_PARENT_HOST, self.Ah, self.traceId)
            _pinpointPy.add_clue(PP_PARENT_HOST, self.Ah, self.traceId)

        # Not nginx, no http
        if PP_HEADER_PINPOINT_PSPANID in request.headers:
            _pinpointPy.add_clue(PP_PARENT_SPAN_ID, request.headers[PP_HEADER_PINPOINT_PSPANID], self.traceId)

        if PP_HEADER_PINPOINT_PAPPNAME in request.headers:
            self.pname = request.headers[PP_HEADER_PINPOINT_PAPPNAME]
            _pinpointPy.set_context_key(PP_PARENT_NAME, self.pname, self.traceId)
            _pinpointPy.add_clue(PP_PARENT_NAME, self.pname, self.traceId)

        if PP_HEADER_PINPOINT_PAPPTYPE in request.headers:
            self.ptype = request.headers[PP_HEADER_PINPOINT_PAPPTYPE]
            _pinpointPy.set_context_key(PP_PARENT_TYPE, self.ptype, self.traceId)
            _pinpointPy.add_clue(PP_PARENT_TYPE, self.ptype, self.traceId)

        if PP_HEADER_PINPOINT_HOST in request.headers:
            self.Ah = request.headers[PP_HEADER_PINPOINT_HOST]
            _pinpointPy.set_context_key(PP_PARENT_HOST, self.Ah, self.traceId)
            _pinpointPy.add_clue(PP_PARENT_HOST, self.Ah, self.traceId)

        if PP_NGINX_PROXY in request.headers:
            _pinpointPy.add_clue(PP_NGINX_PROXY, request.headers[PP_NGINX_PROXY], self.traceId)

        if PP_APACHE_PROXY in request.headers:
            _pinpointPy.add_clue(PP_APACHE_PROXY, request.headers[PP_APACHE_PROXY], self.traceId)

        _pinpointPy.set_context_key("Pinpoint-Sampled", "s1", self.traceId)
        if (PP_HEADER_PINPOINT_SAMPLED in request.headers and request.headers[
            PP_HEADER_PINPOINT_SAMPLED] == PP_NOT_SAMPLED) or _pinpointPy.check_tracelimit():
            if request.headers[PP_HEADER_PINPOINT_SAMPLED] == PP_NOT_SAMPLED:
                _pinpointPy.drop_trace(self.traceId)
                _pinpointPy.set_context_key("Pinpoint-Sampled", "s0", self.traceId)

        _pinpointPy.add_clue(PP_TRANSCATION_ID, self.tid, self.traceId)
        _pinpointPy.add_clue(PP_SPAN_ID, self.sid, self.traceId)
        _pinpointPy.set_context_key(PP_TRANSCATION_ID, self.tid, self.traceId)
        _pinpointPy.set_context_key(PP_SPAN_ID, self.sid, self.traceId)
        return args, kwargs

    def onEnd(self, ret):
        super().onEnd(ret)
        self.isLimit = False
        return ret

    def onException(self, e):
        _pinpointPy.mark_as_error(str(e), "", 0, self.traceId)
        raise e
