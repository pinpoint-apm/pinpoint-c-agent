#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 3/5/20

# ------------------------------------------------------------------------------
#  Copyright  2020. NAVER Corp.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
# ------------------------------------------------------------------------------



import pinpointPy

from pinpoint.common import *
from pinpoint.settings import *

class BaseHTTPRequestPlugins(Candy):
    def __init__(self,name):
        super(BaseHTTPRequestPlugins,self).__init__(name)

    def onBefore(self,*args, **kwargs):
        args, kwargs = super(BaseHTTPRequestPlugins,self).onBefore(*args, **kwargs)
        pinpointPy.add_clue(PP_APP_NAME,APP_NAME)
        pinpointPy.add_clue(PP_APP_ID, APP_ID)
        ###############################################################
        insBaseHttp = args[0]
        pinpointPy.add_clue(PP_INTERCEPTOR_NAME, 'BaseHTTPRequest request')
        pinpointPy.add_clue(PP_REQ_URI,insBaseHttp.path)
        pinpointPy.add_clue(PP_REQ_CLIENT,insBaseHttp.client_address[0])
        
        pinpointPy.add_clue(PP_REQ_SERVER,insBaseHttp.headers.get('Host'))
        pinpointPy.add_clue(PP_SERVER_TYPE,PYTHON)

        sid =  generateSid()
        tid =  generateTid()
        pinpointPy.add_clue(PP_TRANSCATION_ID,tid)
        pinpointPy.add_clue(PP_SPAN_ID,sid)
        pinpointPy.set_context_key(PP_TRANSCATION_ID, tid)
        pinpointPy.set_context_key(PP_SPAN_ID, sid)
        ###############################################################
        return args, kwargs

    def onEnd(self,ret):
        ###############################################################

        ###############################################################
        super(BaseHTTPRequestPlugins,self).onEnd(ret)
        return ret



