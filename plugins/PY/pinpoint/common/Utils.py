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

# Created by eeliu at 11/10/20

import pinpointPy
from .Span import *

def startPinpointByEnviron(environ):
    pinpointPy.add_clue(PP_APP_NAME, APP_NAME)
    pinpointPy.add_clue(PP_APP_ID, APP_ID)
    ###############################################################
    # print("------------------- call before -----------------------")
    pinpointPy.add_clue(PP_INTERCEPTOR_NAME, 'WSGI handle')

    path,remote_addr,host = environ['PATH_INFO'],environ['REMOTE_ADDR'],environ['HTTP_HOST']

    pinpointPy.add_clue(PP_REQ_URI, path)
    pinpointPy.add_clue(PP_REQ_CLIENT, remote_addr)
    pinpointPy.add_clue(PP_REQ_SERVER, host)
    pinpointPy.add_clue(PP_SERVER_TYPE, PYTHON)

    # nginx add http
    if PP_HTTP_PINPOINT_PSPANID in environ:
        pinpointPy.add_clue(PP_PARENT_SPAN_ID, environ[PP_HTTP_PINPOINT_PSPANID])

    if PP_HTTP_PINPOINT_SPANID in environ:
        sid = environ[PP_HTTP_PINPOINT_SPANID]
    elif PP_HEADER_PINPOINT_SPANID in environ:
        sid = environ[PP_HEADER_PINPOINT_SPANID]
    else:
        sid = generateSid()


    if PP_HTTP_PINPOINT_TRACEID in environ:
        tid = environ[PP_HTTP_PINPOINT_TRACEID]
    elif PP_HEADER_PINPOINT_TRACEID in environ:
        tid = environ[PP_HEADER_PINPOINT_TRACEID]
    else:
        tid = generateTid()


    if PP_HTTP_PINPOINT_PAPPNAME in environ:
        pname = environ[PP_HTTP_PINPOINT_PAPPNAME]
        pinpointPy.set_context_key(PP_PARENT_NAME, pname)
        pinpointPy.add_clue(PP_PARENT_NAME, pname)

    if PP_HTTP_PINPOINT_PAPPTYPE in environ:
        ptype = environ[PP_HTTP_PINPOINT_PAPPTYPE]
        pinpointPy.set_context_key(PP_PARENT_TYPE, ptype)
        pinpointPy.add_clue(PP_PARENT_TYPE, ptype)

    if PP_HTTP_PINPOINT_HOST in environ:
        Ah = environ[PP_HTTP_PINPOINT_HOST]
        pinpointPy.set_context_key(PP_PARENT_HOST, Ah)
        pinpointPy.add_clue(PP_PARENT_HOST, Ah)

    # Not nginx, no http
    if PP_HEADER_PINPOINT_PSPANID in environ:
        pinpointPy.add_clue(PP_PARENT_SPAN_ID, environ[PP_HEADER_PINPOINT_PSPANID])
        # print("PINPOINT_PSPANID:", environ[PP_HEADER_PINPOINT_PSPANID])

    if PP_HEADER_PINPOINT_PAPPNAME in environ:
        pname = environ[PP_HEADER_PINPOINT_PAPPNAME]
        pinpointPy.set_context_key(PP_PARENT_NAME, pname)
        pinpointPy.add_clue(PP_PARENT_NAME, pname)

    if PP_HEADER_PINPOINT_PAPPTYPE in environ:
        ptype = environ[PP_HEADER_PINPOINT_PAPPTYPE]
        pinpointPy.set_context_key(PP_PARENT_TYPE, ptype)
        pinpointPy.add_clue(PP_PARENT_TYPE, ptype)

    if PP_HEADER_PINPOINT_HOST in environ:
        Ah = environ[PP_HEADER_PINPOINT_HOST]
        pinpointPy.set_context_key(PP_PARENT_HOST, Ah)
        pinpointPy.add_clue(PP_PARENT_HOST, Ah)

    if PP_NGINX_PROXY in environ:
        pinpointPy.add_clue(PP_NGINX_PROXY, environ[PP_NGINX_PROXY])

    if PP_APACHE_PROXY in environ:
        pinpointPy.add_clue(PP_APACHE_PROXY, environ[PP_APACHE_PROXY])

    pinpointPy.set_context_key(PP_HEADER_PINPOINT_SAMPLED,"s1")
    if (PP_HTTP_PINPOINT_SAMPLED in environ and environ[PP_HTTP_PINPOINT_SAMPLED] == PP_NOT_SAMPLED) or pinpointPy.check_tracelimit():
        pinpointPy.drop_trace()
        pinpointPy.set_context_key(PP_HEADER_PINPOINT_SAMPLED, "s0")


    pinpointPy.add_clue(PP_TRANSCATION_ID,tid)
    pinpointPy.add_clue(PP_SPAN_ID, sid)
    pinpointPy.set_context_key(PP_TRANSCATION_ID, tid)
    pinpointPy.set_context_key(PP_SPAN_ID, sid)

def endPinpointByEnviron(ret):
    # for future use
    pass
