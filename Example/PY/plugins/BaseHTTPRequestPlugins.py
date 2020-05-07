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


from plugins.PinpointCommon import *
from plugins.requests import *

from http.server import BaseHTTPRequestHandler
import traceback
import pinpointPy

class BaseHTTPRequestPlugins(Candy):
    def __init__(self,class_name,module_name):
        super().__init__(class_name,module_name)
        self.isLimit = False

    def onBefore(self,*args, **kwargs):
        pinpointPy.add_clue('appname',APP_NAME)
        pinpointPy.add_clue('appid', APP_ID)
        args, kwargs = super().onBefore(*args, **kwargs)
        ###############################################################
        print("------------------- call before -----------------------")
        insBaseHttp = args[0]
        assert isinstance(insBaseHttp,BaseHTTPRequestHandler)
        pinpointPy.add_clue('name', 'BaseHTTPRequest request')
        pinpointPy.add_clue('uri',insBaseHttp.path)
        pinpointPy.add_clue('client',insBaseHttp.client_address[0])
        pinpointPy.add_clue('server',insBaseHttp.headers.get('Host'))
        pinpointPy.add_clue('stp',PYTHON)

        # nginx add http
        if HTTP_PINPOINT_PSPANID in insBaseHttp.headers:
            pinpointPy.add_clue('psid', insBaseHttp.headers[HTTP_PINPOINT_PSPANID])
            print("PINPOINT_PSPANID:", insBaseHttp.headers[HTTP_PINPOINT_PSPANID])

        if HTTP_PINPOINT_SPANID in insBaseHttp.headers:
            self.sid = insBaseHttp.headers[HTTP_PINPOINT_SPANID]
        elif PINPOINT_SPANID in insBaseHttp.headers:
            self.sid = insBaseHttp.headers[PINPOINT_SPANID]
        else:
            self.sid = self.generateSid()
        pinpointPy.set_special_key('sid',self.sid)


        if HTTP_PINPOINT_TRACEID in insBaseHttp.headers:
            self.tid = insBaseHttp.headers[HTTP_PINPOINT_TRACEID]
        elif PINPOINT_TRACEID in insBaseHttp.headers:
            self.tid = insBaseHttp.headers[PINPOINT_TRACEID]
        else:
            self.tid = self.generateTid()
        pinpointPy.set_special_key('tid',self.tid)

        if HTTP_PINPOINT_PAPPNAME in insBaseHttp.headers:
            self.pname = insBaseHttp.headers[HTTP_PINPOINT_PAPPNAME]
            pinpointPy.set_special_key('pname',self.pname)
            pinpointPy.add_clue('pname',self.pname)

        if HTTP_PINPOINT_PAPPTYPE in insBaseHttp.headers:
            self.ptype = insBaseHttp.headers[HTTP_PINPOINT_PAPPTYPE]
            pinpointPy.set_special_key('ptype',self.ptype)
            pinpointPy.add_clue('ptype',self.ptype)

        if HTTP_PINPOINT_HOST in insBaseHttp.headers:
            self.Ah = insBaseHttp.headers[HTTP_PINPOINT_HOST]
            pinpointPy.set_special_key('Ah',self.Ah)
            pinpointPy.add_clue('Ah',self.Ah)

        # Not nginx, no http
        if PINPOINT_PSPANID in insBaseHttp.headers:
            pinpointPy.add_clue('psid', insBaseHttp.headers[PINPOINT_PSPANID])
            print("PINPOINT_PSPANID:", insBaseHttp.headers[PINPOINT_PSPANID])

        if PINPOINT_PAPPNAME in insBaseHttp.headers:
            self.pname = insBaseHttp.headers[PINPOINT_PAPPNAME]
            pinpointPy.set_special_key('pname', self.pname)
            pinpointPy.add_clue('pname', self.pname)

        if PINPOINT_PAPPTYPE in insBaseHttp.headers:
            self.ptype = insBaseHttp.headers[PINPOINT_PAPPTYPE]
            pinpointPy.set_special_key('ptype', self.ptype)
            pinpointPy.add_clue('ptype', self.ptype)

        if PINPOINT_HOST in insBaseHttp.headers:
            self.Ah = insBaseHttp.headers[PINPOINT_HOST]
            pinpointPy.set_special_key('Ah', self.Ah)
            pinpointPy.add_clue('Ah', self.Ah)

        if NGINX_PROXY in insBaseHttp.headers:
            pinpointPy.add_clue('NP',insBaseHttp.headers[NGINX_PROXY])
        
        if APACHE_PROXY in insBaseHttp.headers:
            pinpointPy.add_clue('AP',insBaseHttp.headers[APACHE_PROXY])

        if SAMPLED in insBaseHttp.headers:
            if insBaseHttp.headers[SAMPLED] == 's0':
                self.isLimit = True
                pinpointPy.drop_trace()
                pinpointPy.set_special_key(SAMPLED,'s0')
        else:
            if pinpointPy.check_tracelimit():
                self.isLimit = True
                pinpointPy.set_special_key(SAMPLED, 's0')
            else:
                self.isLimit = False
                pinpointPy.set_special_key(SAMPLED, 's1')

        pinpointPy.add_clue('tid',self.tid)
        pinpointPy.add_clue('sid',self.sid)
        ###############################################################
        return args, kwargs

    def onEnd(self,ret):
        ###############################################################
        if self.isLimit:
            pinpointPy.drop_trace()
        print("------------------- call end -----------------------")
        ###############################################################
        super().onEnd(ret)
        self.isLimit = False
        return ret

    def onException(self, e):
        pinpointPy.mark_as_error(traceback.format_exc(),"",0)
        raise e



