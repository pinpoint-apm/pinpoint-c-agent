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


from .PinpointAsyCommon import *

import _pinpointPy
import tornado.web

class HTTPRequestPlugins(AsyCandy):
    def __init__(self,class_name,module_name):
        super().__init__(class_name,module_name)
        self.isLimit = False

    def onBefore(self,*args, **kwargs):
        super().onBefore(*args, **kwargs)
        _pinpointPy.add_clue(PP_APP_NAME,APP_NAME,self.node_id)
        _pinpointPy.add_clue(PP_APP_ID, APP_ID,self.node_id)
        _pinpointPy.set_context_key(PP_APP_NAME, APP_NAME,self.node_id)
        ###############################################################
        insBaseHttp = args[0]
        assert isinstance(insBaseHttp,tornado.web.RequestHandler)
        _pinpointPy.add_clue(PP_INTERCEPTOR_NAME, 'tornado.web.RequestHandler',self.node_id)
        _pinpointPy.add_clue(PP_REQ_URI,insBaseHttp.request.uri,self.node_id)
        _pinpointPy.add_clue(PP_REQ_CLIENT,insBaseHttp.request.remote_ip,self.node_id)
        _pinpointPy.add_clue(PP_REQ_SERVER,insBaseHttp.request.host_name,self.node_id)
        _pinpointPy.add_clue(PP_SERVER_TYPE,PYTHON,self.node_id)
        _pinpointPy.set_context_key(PP_SERVER_TYPE, PYTHON,self.node_id)
        insRequest = insBaseHttp.request
        # nginx add http
        if PP_HTTP_PINPOINT_PSPANID in insRequest.headers:
            _pinpointPy.add_clue(PP_PARENT_SPAN_ID, insRequest.headers[PP_HTTP_PINPOINT_PSPANID], self.node_id)

        if PP_HTTP_PINPOINT_SPANID in insRequest.headers:
            self.sid = insRequest.headers[PP_HTTP_PINPOINT_SPANID]
        elif PP_HEADER_PINPOINT_SPANID in insRequest.headers:
            self.sid = insRequest.headers[PP_HEADER_PINPOINT_SPANID]
        else:
            self.sid = self.generateSid()
        _pinpointPy.set_context_key(PP_SPAN_ID,self.sid,self.node_id)

        if PP_HTTP_PINPOINT_TRACEID in insRequest.headers:
            self.tid = insRequest.headers[PP_HTTP_PINPOINT_TRACEID]
        elif PP_HEADER_PINPOINT_TRACEID in insRequest.headers:
            self.tid = insRequest.headers[PP_HEADER_PINPOINT_TRACEID]
        else:
            self.tid = self.generateTid()
        _pinpointPy.set_context_key(PP_TRANSCATION_ID,self.tid,self.node_id)

        if PP_HTTP_PINPOINT_PAPPNAME in insRequest.headers:
            self.pname = insRequest.headers[PP_HTTP_PINPOINT_PAPPNAME]
            _pinpointPy.set_context_key(PP_PARENT_NAME,self.pname,self.node_id)
            _pinpointPy.add_clue(PP_PARENT_NAME,self.pname,self.node_id)

        if PP_HTTP_PINPOINT_PAPPTYPE in insRequest.headers:
            self.ptype = insRequest.headers[PP_HTTP_PINPOINT_PAPPTYPE]
            _pinpointPy.set_context_key(PP_PARENT_TYPE,self.ptype,self.node_id)
            _pinpointPy.add_clue(PP_PARENT_TYPE,self.ptype,self.node_id)

        if PP_HTTP_PINPOINT_HOST in insRequest.headers:
            self.Ah = insRequest.headers[PP_HTTP_PINPOINT_HOST]
            _pinpointPy.set_context_key(PP_PARENT_HOST,self.Ah,self.node_id)
            _pinpointPy.add_clue(PP_PARENT_HOST,self.Ah,self.node_id)

        # Not nginx, no http
        if PP_HEADER_PINPOINT_PSPANID in insRequest.headers:
            _pinpointPy.add_clue(PP_PARENT_SPAN_ID, insRequest.headers[PP_HEADER_PINPOINT_PSPANID], self.node_id)

        if PP_HEADER_PINPOINT_PAPPNAME in insRequest.headers:
            self.pname = insRequest.headers[PP_HEADER_PINPOINT_PAPPNAME]
            _pinpointPy.set_context_key(PP_PARENT_NAME, self.pname,self.node_id)
            _pinpointPy.add_clue(PP_PARENT_NAME, self.pname,self.node_id)

        if PP_HEADER_PINPOINT_PAPPTYPE in insRequest.headers:
            self.ptype = insRequest.headers[PP_HEADER_PINPOINT_PAPPTYPE]
            _pinpointPy.set_context_key(PP_PARENT_TYPE, self.ptype,self.node_id)
            _pinpointPy.add_clue(PP_PARENT_TYPE, self.ptype,self.node_id)

        if PP_HEADER_PINPOINT_HOST in insRequest.headers:
            self.Ah = insRequest.headers[PP_HEADER_PINPOINT_HOST]
            _pinpointPy.set_context_key(PP_PARENT_HOST, self.Ah,self.node_id)
            _pinpointPy.add_clue(PP_PARENT_HOST, self.Ah,self.node_id)

        if PP_NGINX_PROXY in insRequest.headers:
            _pinpointPy.add_clue(PP_NGINX_PROXY, insRequest.headers[PP_NGINX_PROXY], self.node_id)

        if PP_APACHE_PROXY in insRequest.headers:
            _pinpointPy.add_clue(PP_APACHE_PROXY, insRequest.headers[PP_APACHE_PROXY], self.node_id)

        if PP_HEADER_PINPOINT_SAMPLED in insRequest.headers:
            if insRequest.headers[PP_HEADER_PINPOINT_SAMPLED] == PP_NOT_SAMPLED:
                self.isLimit = True
                _pinpointPy.drop_trace(self.node_id)
                _pinpointPy.set_context_key(PP_HEADER_PINPOINT_SAMPLED,PP_NOT_SAMPLED,self.node_id)
        else:
            if _pinpointPy.check_tracelimit():
                self.isLimit = True
                _pinpointPy.set_context_key(PP_HEADER_PINPOINT_SAMPLED, PP_NOT_SAMPLED,self.node_id)
            else:
                self.isLimit = False
                _pinpointPy.set_context_key(PP_HEADER_PINPOINT_SAMPLED, PP_SAMPLED,self.node_id)

        _pinpointPy.add_clue(PP_TRANSCATION_ID,self.tid,self.node_id)
        _pinpointPy.add_clue(PP_SPAN_ID,self.sid,self.node_id)
        ###############################################################
        return args, kwargs

    def onEnd(self,ret):
        ###############################################################
        if self.isLimit:
            _pinpointPy.drop_trace(self.node_id)
        ###############################################################
        super().onEnd(ret)
        self.isLimit = False
        return ret

    def onException(self, e):
        _pinpointPy.mark_as_error(str(e),"",0,self.node_id)
        raise e



