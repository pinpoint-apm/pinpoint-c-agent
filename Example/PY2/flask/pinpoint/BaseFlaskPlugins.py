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
from .plugins import *

from flask import Flask,Request
import threading


class BaseFlaskPlugins(Candy):
    def __init__(self,class_name,module_name):
        super(BaseFlaskPlugins,self).__init__(class_name,module_name)
        self.isLimit = False

    def onBefore(self,*args, **kwargs):
        args, kwargs = super(BaseFlaskPlugins,self).onBefore(*args, **kwargs)
        request = Request(args[1])
        pinpointPy.add_clue('appname',APP_NAME)
        pinpointPy.add_clue('appid', APP_ID)
        ###############################################################
        # print(threading.currentThread().ident)
        print("------------------- call before -----------------------")
        pinpointPy.add_clue('name', 'BaseFlaskrequest')
        pinpointPy.add_clue('uri', request.path)
        pinpointPy.add_clue('client',request.remote_addr)
        pinpointPy.add_clue('server', request.host)
        pinpointPy.add_clue('stp', PYTHON)

        # nginx add http
        if HTTP_PINPOINT_PSPANID in request.headers:
            pinpointPy.add_clue('psid', request.headers[HTTP_PINPOINT_PSPANID])
            print("PINPOINT_PSPANID:", request.headers[HTTP_PINPOINT_PSPANID])

        if HTTP_PINPOINT_SPANID in request.headers:
            self.sid = request.headers[HTTP_PINPOINT_SPANID]
        elif PINPOINT_SPANID in request.headers:
            self.sid = request.headers[PINPOINT_SPANID]
        else:
            self.sid = self.generateSid()
        pinpointPy.set_special_key('sid',self.sid)


        if HTTP_PINPOINT_TRACEID in request.headers:
            self.tid = request.headers[HTTP_PINPOINT_TRACEID]
        elif PINPOINT_TRACEID in request.headers:
            self.tid = request.headers[PINPOINT_TRACEID]
        else:
            self.tid = self.generateTid()
        pinpointPy.set_special_key('tid',self.tid)

        if HTTP_PINPOINT_PAPPNAME in request.headers:
            self.pname = request.headers[HTTP_PINPOINT_PAPPNAME]
            pinpointPy.set_special_key('pname',self.pname)
            pinpointPy.add_clue('pname',self.pname)

        if HTTP_PINPOINT_PAPPTYPE in request.headers:
            self.ptype = request.headers[HTTP_PINPOINT_PAPPTYPE]
            pinpointPy.set_special_key('ptype',self.ptype)
            pinpointPy.add_clue('ptype',self.ptype)

        if HTTP_PINPOINT_HOST in request.headers:
            self.Ah = request.headers[HTTP_PINPOINT_HOST]
            pinpointPy.set_special_key('Ah',self.Ah)
            pinpointPy.add_clue('Ah',self.Ah)

        # Not nginx, no http
        if PINPOINT_PSPANID in request.headers:
            pinpointPy.add_clue('psid', request.headers[PINPOINT_PSPANID])
            print("PINPOINT_PSPANID:", request.headers[PINPOINT_PSPANID])

        if PINPOINT_PAPPNAME in request.headers:
            self.pname = request.headers[PINPOINT_PAPPNAME]
            pinpointPy.set_special_key('pname', self.pname)
            pinpointPy.add_clue('pname', self.pname)

        if PINPOINT_PAPPTYPE in request.headers:
            self.ptype = request.headers[PINPOINT_PAPPTYPE]
            pinpointPy.set_special_key('ptype', self.ptype)
            pinpointPy.add_clue('ptype', self.ptype)

        if PINPOINT_HOST in request.headers:
            self.Ah = request.headers[PINPOINT_HOST]
            pinpointPy.set_special_key('Ah', self.Ah)
            pinpointPy.add_clue('Ah', self.Ah)
        
        if NGINX_PROXY in request.headers:
            pinpointPy.add_clue('NP',request.headers[NGINX_PROXY])
        
        if APACHE_PROXY in request.headers:
            pinpointPy.add_clue('AP',request.headers[APACHE_PROXY])

        if SAMPLED in request.headers:
            if request.headers[SAMPLED] == 's0':
                self.isLimit = True
                pinpointPy.drop_trace()
        else:
            pinpointPy.check_tracelimit()
            # print(self.isLimit)
        pinpointPy.add_clue('tid',self.tid)
        pinpointPy.add_clue('sid',self.sid)
        ###############################################################
        return args, kwargs

    def onEnd(self,ret):
        ###############################################################

        print("------------------- call end -----------------------")

        ###############################################################
        super(BaseFlaskPlugins,self).onEnd(ret)
        self.isLimit = False
        return ret

    def onException(self, e):
        import traceback
        pinpointPy.mark_as_error(traceback.format_exc(),"",0)
        raise e
