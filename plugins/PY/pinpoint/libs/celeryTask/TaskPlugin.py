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


import pinpointPy
from pinpoint.common import *

class TaskPlugin(Candy):

    def __init__(self, name):
        super().__init__(name)
        self.type= Candy.E_PER_REQ

    def isSample(self):
        if pinpointPy.trace_has_root():# no trace
            self.type = Candy.E_FUNCTION 
        else:
            self.type = Candy.E_PER_REQ
        return True

    def onBefore(self, *args, **kwargs):
        if self.type == Candy.E_FUNCTION:
            return self.funcOnBefore(*args, **kwargs)
        else:
            return self.perRequestOnBefore(*args, **kwargs)

    def onEnd(self, ret):
        if self.type == Candy.E_FUNCTION:
            return self.funcOnEnd(ret)
        else:
            return self.onRequestOnEnd(ret)

    def funcOnBefore(self,*args, **kwargs):
        super().onBefore(*args, **kwargs)
        ###############################################################

        pinpointPy.add_clue(PP_INTERCEPTOR_NAME, self.getFuncUniqueName())
        pinpointPy.add_clue(PP_SERVER_TYPE, PP_CELERY)

        ###############################################################
        return args, kwargs

    def funcOnEnd(self,ret):
        ###############################################################
        pinpointPy.add_clues(PP_RETURN, str(ret))
        ###############################################################
        super().onEnd(ret)
        return ret


    def perRequestOnBefore(self, *args, **kwargs):
        super().onBefore(*args, **kwargs)
        APP_NAME = 'celery-task-queue'
        taskName = args[3]['task']
        APP_ID = taskName[0:24]

        pinpointPy.add_clue(PP_APP_NAME, APP_NAME)
        pinpointPy.add_clue(PP_APP_ID, APP_ID)
        pinpointPy.set_context_key(PP_APP_NAME,APP_NAME)
        ###############################################################

        headers= args[3]
        pinpointPy.add_clue(PP_INTERCEPTOR_NAME, 'worker')
        pinpointPy.add_clue(PP_REQ_URI, APP_ID)
        if 'hostname' in headers:
            pinpointPy.add_clue(PP_REQ_SERVER,headers['hostname'])
        # use PP_RABBITMQ_CLIENT as default
        serverType = PP_RABBITMQ_CLIENT

        if PP_HEADER_PINPOINT_SPANID in headers:
            self.sid = headers[PP_HEADER_PINPOINT_SPANID]
        else:
            self.sid = generateSid()
        pinpointPy.set_context_key(PP_SPAN_ID, self.sid)

        if PP_HEADER_PINPOINT_TRACEID in headers:
            self.tid = headers[PP_HEADER_PINPOINT_TRACEID]
        else:
            self.tid = generateTid()
        pinpointPy.set_context_key(PP_TRANSCATION_ID, self.tid)
        
        if 'delivery_info' in headers:
            pinpointPy.add_clue(PP_PARENT_NAME, "celery-beat")
            pinpointPy.add_clue(PP_REQ_CLIENT, "Unknown")
            if 'origin' in headers:
                pinpointPy.add_clue(PP_PARENT_HOST, headers['origin'])
            if 'exchange' in headers:
                pinpointPy.add_clue(PP_REQ_URI,"rabbitmq://exchange="+headers['exchange'])
            if 'routing_key' in headers:
                pinpointPy.add_clues(PP_RABBITMQ_ROUTINGKEY,headers['routing_key'])
            # set span id zero
            pinpointPy.add_clue(PP_PARENT_SPAN_ID, '0')
        
        if PP_HEADER_PINPOINT_CLIENT in headers:
            pinpointPy.add_clue(PP_REQ_CLIENT, headers[PP_HEADER_PINPOINT_CLIENT])

        if PP_HTTP_PINPOINT_PAPPNAME in headers:
            self.pname = headers[PP_HTTP_PINPOINT_PAPPNAME]
            pinpointPy.set_context_key(PP_PARENT_NAME, self.pname)
            pinpointPy.add_clue(PP_PARENT_NAME, self.pname)

        if PP_HTTP_PINPOINT_PAPPTYPE in headers:
            self.ptype = headers[PP_HTTP_PINPOINT_PAPPTYPE]
            pinpointPy.set_context_key(PP_PARENT_TYPE, self.ptype)
            pinpointPy.add_clue(PP_PARENT_TYPE, self.ptype)
            pinpointPy.add_clue(PP_SERVER_TYPE, self.ptype)

        if PP_HEADER_PINPOINT_HOST in headers:
            self.Ah = headers[PP_HEADER_PINPOINT_HOST]
            pinpointPy.set_context_key(PP_PARENT_HOST, self.Ah)
            pinpointPy.add_clue(PP_PARENT_HOST, self.Ah)

        if PP_HEADER_PINPOINT_PSPANID in headers:
            pinpointPy.add_clue(PP_PARENT_SPAN_ID, headers[PP_HEADER_PINPOINT_PSPANID])

        if PP_HEADER_PINPOINT_PAPPNAME in headers:
            self.pname = headers[PP_HEADER_PINPOINT_PAPPNAME]
            pinpointPy.set_context_key(PP_PARENT_NAME, self.pname)
            pinpointPy.add_clue(PP_PARENT_NAME, self.pname)

        if PP_HEADER_PINPOINT_PAPPTYPE in headers:
            self.ptype = headers[PP_HEADER_PINPOINT_PAPPTYPE]
            pinpointPy.set_context_key(PP_PARENT_TYPE, self.ptype)
            pinpointPy.add_clue(PP_PARENT_TYPE, self.ptype)
            serverType = PYTHON

        pinpointPy.set_context_key(PP_HEADER_PINPOINT_SAMPLED, "s1")
        if (PP_HEADER_PINPOINT_SAMPLED in headers and headers[
            PP_HEADER_PINPOINT_SAMPLED] == PP_NOT_SAMPLED) or pinpointPy.check_tracelimit():
            pinpointPy.drop_trace()
            pinpointPy.set_context_key(PP_HEADER_PINPOINT_SAMPLED, "s0")

        pinpointPy.add_clue(PP_TRANSCATION_ID, self.tid)
        pinpointPy.add_clue(PP_SPAN_ID, self.sid)
        pinpointPy.set_context_key(PP_TRANSCATION_ID, self.tid)
        pinpointPy.set_context_key(PP_SPAN_ID, self.sid)
        pinpointPy.add_clue(PP_SERVER_TYPE, serverType)
        pinpointPy.set_context_key(PP_SERVER_TYPE,serverType)

        ###############################################################
        return args, kwargs

    def onRequestOnEnd(self, ret):
        ###############################################################
        # if ret:
            # pinpointPy.add_clues(PP_HTTP_STATUS_CODE, str(ret.status_code))
        ###############################################################
        super().onEnd(ret)
        return ret

    def onException(self, e):
        pinpointPy.mark_as_error(str(e), "", 0)
        raise e