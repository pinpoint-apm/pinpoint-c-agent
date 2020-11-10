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


from pinpoint.plugins import *

import traceback
import pinpointPy

class BaseDjangoRequestPlugins(Candy):
    def __init__(self,class_name,module_name):
        super().__init__(class_name,module_name)
        self.isLimit = False

    def onBefore(self,*args, **kwargs):
        args, kwargs = super().onBefore(*args, **kwargs)
        pinpointPy.add_clue(PP_APP_NAME,APP_NAME)
        pinpointPy.add_clue(PP_APP_ID, APP_ID)
        ###############################################################
        request = args[1]
        # assert isinstance(request,BaseHTTPRequestHandler)
        pinpointPy.add_clue(PP_INTERCEPTER_NAME, 'BaseDjangoRequest request')
        pinpointPy.add_clue(PP_REQ_URI,request.path)
        # print(request.META)
        pinpointPy.add_clue(PP_REQ_CLIENT,request.META['REMOTE_ADDR'])
        pinpointPy.add_clue(PP_REQ_SERVER,request.get_host())
        pinpointPy.add_clue(PP_SERVER_TYPE,PYTHON)

        # nginx add http
        if HTTP_PINPOINT_PSPANID in request.META:
            pinpointPy.add_clue(PP_PARENT_SPAN_ID, request.META[HTTP_PINPOINT_PSPANID])
            print("PINPOINT_PSPANID:", request.META[HTTP_PINPOINT_PSPANID])

        if HTTP_PINPOINT_SPANID in request.META:
            self.sid = request.META[HTTP_PINPOINT_SPANID]
        elif PINPOINT_SPANID in request.META:
            self.sid = request.META[PINPOINT_SPANID]
        else:
            self.sid = generateSid()
        pinpointPy.set_special_key(PP_SPAN_ID,self.sid)


        if HTTP_PINPOINT_TRACEID in request.META:
            self.tid = request.META[HTTP_PINPOINT_TRACEID]
        elif PINPOINT_TRACEID in request.META:
            self.tid = request.META[PINPOINT_TRACEID]
        else:
            self.tid = generateTid()
        pinpointPy.set_special_key(PP_TRANSCATION_ID,self.tid)

        if HTTP_PINPOINT_PAPPNAME in request.META:
            self.pname = request.META[HTTP_PINPOINT_PAPPNAME]
            pinpointPy.set_special_key(PP_PARENT_NAME,self.pname)
            pinpointPy.add_clue(PP_PARENT_NAME,self.pname)

        if HTTP_PINPOINT_PAPPTYPE in request.META:
            self.ptype = request.META[HTTP_PINPOINT_PAPPTYPE]
            pinpointPy.set_special_key(PP_PARENT_TYPE,self.ptype)
            pinpointPy.add_clue(PP_PARENT_TYPE,self.ptype)

        if HTTP_PINPOINT_HOST in request.META:
            self.Ah = request.META[HTTP_PINPOINT_HOST]
            pinpointPy.set_special_key(PP_PARENT_HOST,self.Ah)
            pinpointPy.add_clue(PP_PARENT_HOST,self.Ah)

        # Not nginx, no http
        if PINPOINT_PSPANID in request.META:
            pinpointPy.add_clue(PP_PARENT_SPAN_ID, request.META[PINPOINT_PSPANID])
            print("PINPOINT_PSPANID:", request.META[PINPOINT_PSPANID])

        if PINPOINT_PAPPNAME in request.META:
            self.pname = request.META[PINPOINT_PAPPNAME]
            pinpointPy.set_special_key(PP_PARENT_NAME, self.pname)
            pinpointPy.add_clue(PP_PARENT_NAME, self.pname)

        if PINPOINT_PAPPTYPE in request.META:
            self.ptype = request.META[PINPOINT_PAPPTYPE]
            pinpointPy.set_special_key(PP_PARENT_TYPE, self.ptype)
            pinpointPy.add_clue(PP_PARENT_TYPE, self.ptype)

        if PINPOINT_HOST in request.META:
            self.Ah = request.META[PINPOINT_HOST]
            pinpointPy.set_special_key(PP_PARENT_HOST, self.Ah)
            pinpointPy.add_clue(PP_PARENT_HOST, self.Ah)

        if NGINX_PROXY in request.META:
            pinpointPy.add_clue(PP_NGINX_PROXY,request.META[NGINX_PROXY])
        elif HTTP_NGINX_PROXY in request.META:
            pinpointPy.add_clue(PP_NGINX_PROXY,request.META[HTTP_NGINX_PROXY])
        
        if APACHE_PROXY in request.META:
            pinpointPy.add_clue(PP_APACHE_PROXY,request.META[APACHE_PROXY])
        elif HTTP_APACHE_PROXY in request.META:
            pinpointPy.add_clue(PP_APACHE_PROXY,request.META[HTTP_APACHE_PROXY])

        if PP_HTTP_SAMPLED in request.META :
            sampled = request.META[PP_HTTP_SAMPLED]
        elif HTTPD_SAMPLED in request.META :
            sampled = request.META[HTTPD_SAMPLED]
        else:
            sampled = PP_NOT_SAMPLED if pinpointPy.check_tracelimit() else PP_SAMPLED

        # sampled = PP_NOT_SAMPLED if pinpointPy.check_tracelimit() else PP_SAMPLED

        if sampled == PP_NOT_SAMPLED:
            self.isLimit = True
            pinpointPy.drop_trace()
            pinpointPy.set_special_key(PP_HTTP_SAMPLED,PP_NOT_SAMPLED)
        else:
            self.isLimit = False
            pinpointPy.set_special_key(PP_HTTP_SAMPLED, PP_SAMPLED)


        # if (PP_HTTP_SAMPLED in request.META and request.META[PP_HTTP_SAMPLED] == PP_NOT_SAMPLED) or \
        #         (HTTPD_SAMPLED in request.META and request.META[HTTPD_SAMPLED] == PP_NOT_SAMPLED):
        #     self.isLimit = True
        #     pinpointPy.drop_trace()
        #     pinpointPy.set_special_key(PP_HTTP_SAMPLED,PP_NOT_SAMPLED)
        # else:
        #     if pinpointPy.check_tracelimit():
        #         self.isLimit = True
        #         pinpointPy.drop_trace()
        #         pinpointPy.set_special_key(PP_HTTP_SAMPLED, PP_NOT_SAMPLED)
        #     else:
        #         self.isLimit = False
        #         pinpointPy.set_special_key(PP_HTTP_SAMPLED, PP_SAMPLED)

        pinpointPy.add_clue(PP_TRANSCATION_ID,self.tid)
        pinpointPy.add_clue(PP_SPAN_ID,self.sid)
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



