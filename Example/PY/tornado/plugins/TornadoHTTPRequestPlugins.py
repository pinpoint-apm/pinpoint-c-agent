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


from plugins.PinpointAsyCommon import *

import traceback
import pinpointPy
import tornado.web

class TornadoHTTPRequestPlugins(AsyCandy):
    def __init__(self,class_name,module_name):
        super().__init__(class_name,module_name)
        self.isLimit = False

    def onBefore(self,*args, **kwargs):
        args, kwargs = super().onBefore(*args, **kwargs)
        pinpointPy.add_clue('appname',APP_NAME)
        pinpointPy.add_clue('appid', APP_ID)
        ###############################################################
        print("------------------- call before -----------------------")
        insBaseHttp = args[0]
        assert isinstance(insBaseHttp,tornado.web.RequestHandler)
        pinpointPy.add_clue('name', 'tornado.web.RequestHandler')
        pinpointPy.add_clue('uri',insBaseHttp.request.uri)
        pinpointPy.add_clue('client',insBaseHttp.request.remote_ip)
        pinpointPy.add_clue('server',insBaseHttp.request.host_name)
        pinpointPy.add_clue('stp',PYTHON)

        insRequest = insBaseHttp.request
        # nginx add http
        if HTTP_PINPOINT_PSPANID in insRequest.headers:
            pinpointPy.add_clue('psid', insRequest.headers[HTTP_PINPOINT_PSPANID])
            print("PINPOINT_PSPANID:", insRequest.headers[HTTP_PINPOINT_PSPANID])

        if HTTP_PINPOINT_SPANID in insRequest.headers:
            self.sid = insRequest.headers[HTTP_PINPOINT_SPANID]
        elif PINPOINT_SPANID in insRequest.headers:
            self.sid = insRequest.headers[PINPOINT_SPANID]
        else:
            self.sid = self.generateSid()
        pinpointPy.set_special_key('sid',self.sid)


        if HTTP_PINPOINT_TRACEID in insRequest.headers:
            self.tid = insRequest.headers[HTTP_PINPOINT_TRACEID]
        elif PINPOINT_TRACEID in insRequest.headers:
            self.tid = insRequest.headers[PINPOINT_TRACEID]
        else:
            self.tid = self.generateTid()
        pinpointPy.set_special_key('tid',self.tid)

        if HTTP_PINPOINT_PAPPNAME in insRequest.headers:
            self.pname = insRequest.headers[HTTP_PINPOINT_PAPPNAME]
            pinpointPy.set_special_key('pname',self.pname)
            pinpointPy.add_clue('pname',self.pname)

        if HTTP_PINPOINT_PAPPTYPE in insRequest.headers:
            self.ptype = insRequest.headers[HTTP_PINPOINT_PAPPTYPE]
            pinpointPy.set_special_key('ptype',self.ptype)
            pinpointPy.add_clue('ptype',self.ptype)

        if HTTP_PINPOINT_HOST in insRequest.headers:
            self.Ah = insRequest.headers[HTTP_PINPOINT_HOST]
            pinpointPy.set_special_key('Ah',self.Ah)
            pinpointPy.add_clue('Ah',self.Ah)

        # Not nginx, no http
        if PINPOINT_PSPANID in insRequest.headers:
            pinpointPy.add_clue('psid', insRequest.headers[PINPOINT_PSPANID])
            print("PINPOINT_PSPANID:", insRequest.headers[PINPOINT_PSPANID])

        if PINPOINT_PAPPNAME in insRequest.headers:
            self.pname = insRequest.headers[PINPOINT_PAPPNAME]
            pinpointPy.set_special_key('pname', self.pname)
            pinpointPy.add_clue('pname', self.pname)

        if PINPOINT_PAPPTYPE in insRequest.headers:
            self.ptype = insRequest.headers[PINPOINT_PAPPTYPE]
            pinpointPy.set_special_key('ptype', self.ptype)
            pinpointPy.add_clue('ptype', self.ptype)

        if PINPOINT_HOST in insRequest.headers:
            self.Ah = insRequest.headers[PINPOINT_HOST]
            pinpointPy.set_special_key('Ah', self.Ah)
            pinpointPy.add_clue('Ah', self.Ah)

        if NGINX_PROXY in insRequest.headers:
            pinpointPy.add_clue('NP',insRequest.headers[NGINX_PROXY])

        if APACHE_PROXY in insRequest.headers:
            pinpointPy.add_clue('AP',insRequest.headers[APACHE_PROXY])

        if SAMPLED in insRequest.headers:
            if insRequest.headers[SAMPLED] == 's0':
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



