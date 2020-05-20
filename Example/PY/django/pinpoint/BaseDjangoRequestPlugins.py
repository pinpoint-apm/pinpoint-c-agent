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


from .plugins import *

import traceback
import pinpointPy

class BaseDjangoRequestPlugins(Candy):
    def __init__(self,class_name,module_name):
        super().__init__(class_name,module_name)
        self.isLimit = False

    def onBefore(self,*args, **kwargs):
        args, kwargs = super().onBefore(*args, **kwargs)
        pinpointPy.add_clue('appname',APP_NAME)
        pinpointPy.add_clue('appid', APP_ID)
        ###############################################################
        print("------------------- call before -----------------------")
        request = args[1]
        # assert isinstance(request,BaseHTTPRequestHandler)
        pinpointPy.add_clue('name', 'BaseDjangoRequest request')
        pinpointPy.add_clue('uri',request.path)
        # print(request.META)
        pinpointPy.add_clue('client',request.META['REMOTE_ADDR'])
        pinpointPy.add_clue('server',request.get_host())
        pinpointPy.add_clue('stp',PYTHON)

        # nginx add http
        if HTTP_PINPOINT_PSPANID in request.META:
            pinpointPy.add_clue('psid', request.META[HTTP_PINPOINT_PSPANID])
            print("PINPOINT_PSPANID:", request.META[HTTP_PINPOINT_PSPANID])

        if HTTP_PINPOINT_SPANID in request.META:
            self.sid = request.META[HTTP_PINPOINT_SPANID]
        elif PINPOINT_SPANID in request.META:
            self.sid = request.META[PINPOINT_SPANID]
        else:
            self.sid = self.generateSid()
        pinpointPy.set_special_key('sid',self.sid)


        if HTTP_PINPOINT_TRACEID in request.META:
            self.tid = request.META[HTTP_PINPOINT_TRACEID]
        elif PINPOINT_TRACEID in request.META:
            self.tid = request.META[PINPOINT_TRACEID]
        else:
            self.tid = self.generateTid()
        pinpointPy.set_special_key('tid',self.tid)

        if HTTP_PINPOINT_PAPPNAME in request.META:
            self.pname = request.META[HTTP_PINPOINT_PAPPNAME]
            pinpointPy.set_special_key('pname',self.pname)
            pinpointPy.add_clue('pname',self.pname)

        if HTTP_PINPOINT_PAPPTYPE in request.META:
            self.ptype = request.META[HTTP_PINPOINT_PAPPTYPE]
            pinpointPy.set_special_key('ptype',self.ptype)
            pinpointPy.add_clue('ptype',self.ptype)

        if HTTP_PINPOINT_HOST in request.META:
            self.Ah = request.META[HTTP_PINPOINT_HOST]
            pinpointPy.set_special_key('Ah',self.Ah)
            pinpointPy.add_clue('Ah',self.Ah)

        # Not nginx, no http
        if PINPOINT_PSPANID in request.META:
            pinpointPy.add_clue('psid', request.META[PINPOINT_PSPANID])
            print("PINPOINT_PSPANID:", request.META[PINPOINT_PSPANID])

        if PINPOINT_PAPPNAME in request.META:
            self.pname = request.META[PINPOINT_PAPPNAME]
            pinpointPy.set_special_key('pname', self.pname)
            pinpointPy.add_clue('pname', self.pname)

        if PINPOINT_PAPPTYPE in request.META:
            self.ptype = request.META[PINPOINT_PAPPTYPE]
            pinpointPy.set_special_key('ptype', self.ptype)
            pinpointPy.add_clue('ptype', self.ptype)

        if PINPOINT_HOST in request.META:
            self.Ah = request.META[PINPOINT_HOST]
            pinpointPy.set_special_key('Ah', self.Ah)
            pinpointPy.add_clue('Ah', self.Ah)

        if NGINX_PROXY in request.META:
            pinpointPy.add_clue('NP',request.META[NGINX_PROXY])
        elif HTTP_NGINX_PROXY in request.META:
            pinpointPy.add_clue('NP',request.META[HTTP_NGINX_PROXY])
        
        if APACHE_PROXY in request.META:
            pinpointPy.add_clue('AP',request.META[APACHE_PROXY])
        elif HTTP_APACHE_PROXY in request.META:
            pinpointPy.add_clue('AP',request.META[HTTP_APACHE_PROXY])

        if SAMPLED in request.META :
            sampled = request.META[SAMPLED]
        elif HTTPD_SAMPLED in request.META :
            sampled = request.META[HTTPD_SAMPLED]
        else:
            sampled = 's0' if pinpointPy.check_tracelimit() else 's1'

        # sampled = 's0' if pinpointPy.check_tracelimit() else 's1'

        if sampled == 's0':
            self.isLimit = True
            pinpointPy.drop_trace()
            pinpointPy.set_special_key(SAMPLED,'s0')
        else:
            self.isLimit = False
            pinpointPy.set_special_key(SAMPLED, 's1')


        # if (SAMPLED in request.META and request.META[SAMPLED] == 's0') or \
        #         (HTTPD_SAMPLED in request.META and request.META[HTTPD_SAMPLED] == 's0'):
        #     self.isLimit = True
        #     pinpointPy.drop_trace()
        #     pinpointPy.set_special_key(SAMPLED,'s0')
        # else:
        #     if pinpointPy.check_tracelimit():
        #         self.isLimit = True
        #         pinpointPy.drop_trace()
        #         pinpointPy.set_special_key(SAMPLED, 's0')
        #     else:
        #         self.isLimit = False
        #         pinpointPy.set_special_key(SAMPLED, 's1')

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



