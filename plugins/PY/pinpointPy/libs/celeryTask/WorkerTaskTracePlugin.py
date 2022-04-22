#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 1/6/21

# ******************************************************************************
#   Copyright  2020. NAVER Corp.
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
# ******************************************************************************


import _pinpointPy
from common import *

# @deprected
class WorkerTaskTracePlugin(PinTrace):

    def __init__(self, name):
        super().__init__(name)
        self.type= PinTrace.E_PER_REQ

    def onBefore(self, *args, **kwargs):
        return self.perRequestOnBefore(*args, **kwargs)

    def onEnd(self, ret):
        return self.onRequestOnEnd(ret)

    def perRequestOnBefore(self, *args, **kwargs):
        super().onBefore(*args, **kwargs)
        APP_NAME = 'celery-task-queue'
        APP_ID = args[0].name[0:24]
        _pinpointPy.add_clue(PP_APP_NAME, APP_NAME)
        _pinpointPy.add_clue(PP_APP_ID, APP_ID)
        _pinpointPy.set_context_key(PP_APP_NAME,APP_NAME)
        ###############################################################
        headers = args[4]
        _pinpointPy.add_clue(PP_INTERCEPTOR_NAME, 'worker')
        _pinpointPy.add_clue(PP_REQ_URI, APP_ID)
        _pinpointPy.add_clue(PP_REQ_SERVER, APP_ID)
        _pinpointPy.add_clue(PP_SERVER_TYPE, PYTHON)
        _pinpointPy.set_context_key(PP_SERVER_TYPE,PYTHON)

        if PP_HEADER_PINPOINT_CLIENT in headers:
            _pinpointPy.add_clue(PP_REQ_CLIENT, headers[PP_HEADER_PINPOINT_CLIENT])

        if PP_HTTP_PINPOINT_SPANID in headers:
            self.sid = headers[PP_HTTP_PINPOINT_SPANID]
        elif PP_HEADER_PINPOINT_SPANID in headers:
            self.sid = headers[PP_HEADER_PINPOINT_SPANID]
        else:
            self.sid = generateSid()
        _pinpointPy.set_context_key(PP_SPAN_ID, self.sid)

        if PP_HTTP_PINPOINT_TRACEID in headers:
            self.tid = headers[PP_HTTP_PINPOINT_TRACEID]
        elif PP_HEADER_PINPOINT_TRACEID in headers:
            self.tid = headers[PP_HEADER_PINPOINT_TRACEID]
        else:
            self.tid = generateTid()
        _pinpointPy.set_context_key(PP_TRANSCATION_ID, self.tid)

        if PP_HTTP_PINPOINT_PAPPNAME in headers:
            self.pname = headers[PP_HTTP_PINPOINT_PAPPNAME]
            _pinpointPy.set_context_key(PP_PARENT_NAME, self.pname)
            _pinpointPy.add_clue(PP_PARENT_NAME, self.pname)

        if PP_HTTP_PINPOINT_PAPPTYPE in headers:
            self.ptype = headers[PP_HTTP_PINPOINT_PAPPTYPE]
            _pinpointPy.set_context_key(PP_PARENT_TYPE, self.ptype)
            _pinpointPy.add_clue(PP_PARENT_TYPE, self.ptype)

        if PP_HTTP_PINPOINT_HOST in headers:
            self.Ah = headers[PP_HTTP_PINPOINT_HOST]
            _pinpointPy.set_context_key(PP_PARENT_HOST, self.Ah)
            _pinpointPy.add_clue(PP_PARENT_HOST, self.Ah)

        if PP_HTTP_PINPOINT_PSPANID in headers:
            _pinpointPy.add_clue(PP_PARENT_SPAN_ID, headers[PP_HTTP_PINPOINT_PSPANID])

        if PP_HEADER_PINPOINT_PAPPNAME in headers:
            self.pname = headers[PP_HEADER_PINPOINT_PAPPNAME]
            _pinpointPy.set_context_key(PP_PARENT_NAME, self.pname)
            _pinpointPy.add_clue(PP_PARENT_NAME, self.pname)

        if PP_HEADER_PINPOINT_PAPPTYPE in headers:
            self.ptype = headers[PP_HEADER_PINPOINT_PAPPTYPE]
            _pinpointPy.set_context_key(PP_PARENT_TYPE, self.ptype)
            _pinpointPy.add_clue(PP_PARENT_TYPE, self.ptype)

        if PP_HEADER_PINPOINT_HOST in headers:
            self.Ah = headers[PP_HEADER_PINPOINT_HOST]
            _pinpointPy.set_context_key(PP_PARENT_HOST, self.Ah)
            _pinpointPy.add_clue(PP_PARENT_HOST, self.Ah)

        _pinpointPy.set_context_key(PP_HEADER_PINPOINT_SAMPLED, "s1")
        if (PP_HTTP_PINPOINT_SAMPLED in headers and headers[
            PP_HTTP_PINPOINT_SAMPLED] == PP_NOT_SAMPLED) or _pinpointPy.check_tracelimit():
            _pinpointPy.drop_trace()
            _pinpointPy.set_context_key(PP_HEADER_PINPOINT_SAMPLED, "s0")

        _pinpointPy.add_clue(PP_TRANSCATION_ID, self.tid)
        _pinpointPy.add_clue(PP_SPAN_ID, self.sid)
        _pinpointPy.set_context_key(PP_TRANSCATION_ID, self.tid)
        _pinpointPy.set_context_key(PP_SPAN_ID, self.sid)

        ###############################################################
        return args, kwargs

    def onRequestOnEnd(self, ret):
        ###############################################################
        # if ret:
            # _pinpointPy.add_clues(PP_HTTP_STATUS_CODE, str(ret.status_code))
        ###############################################################
        super().onEnd(ret)
        return ret

    def onException(self, e):
        _pinpointPy.mark_as_error(str(e), "", 0)
        raise e