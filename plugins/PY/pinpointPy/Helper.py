#!/usr/bin/env python
# -*- coding: UTF-8 -*-

# ------------------------------------------------------------------------------
#  Copyright  2020. NAVER Corp.                                                -
#                                                                              -
#  Licensed under the Apache License, Version 2.0 (the "License");             -
#  you may not use this file except in compliance with the License.            -
#  You may obtain a copy of the License at                                     -
#                                                                              -
#   http://www.apache.org/licenses/LICENSE-2.0        _pinpointPy                         -
#                                                                              -
#  Unless required by applicable law or agreed to in writing, software         -
#  distributed under the License is distributed on an "AS IS" BASIS,           -
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.    -
#  See the License for the specific language governing permissions and         -
#  limitations under the License.                                              -
# ------------------------------------------------------------------------------

# Created by eeliu at 11/10/20
from . import Defines
from . import pinpoint

def generateNextSid():
   return pinpoint.gen_sid()

def generatePinpointHeader(host, headers):
    headers[Defines.PP_HEADER_PINPOINT_SAMPLED] = Defines.PP_SAMPLED
    headers[Defines.PP_HEADER_PINPOINT_PAPPTYPE] = pinpoint.get_context(Defines.PP_SERVER_TYPE)
    headers[Defines.PP_HEADER_PINPOINT_PAPPNAME] = pinpoint.get_context(Defines.PP_APP_NAME)
    headers['Pinpoint-Flags'] = "0"
    headers[Defines.PP_HEADER_PINPOINT_HOST] = host
    headers[Defines.PP_HEADER_PINPOINT_TRACEID] = pinpoint.get_context(Defines.PP_TRANSCATION_ID)
    headers[Defines.PP_HEADER_PINPOINT_PSPANID] = pinpoint.get_context(Defines.PP_SPAN_ID)
    nsid = pinpoint.gen_sid()
    pinpoint.add_context(Defines.PP_NEXT_SPAN_ID, nsid)
    headers[Defines.PP_HEADER_PINPOINT_SPANID] = nsid


# def generatePPRabbitMqHeader(func, headers):
#     if pinpoint.get_context(Defines.PP_HEADER_PINPOINT_SAMPLED) == 's0':
#         headers[Defines.PP_HEADER_PINPOINT_SAMPLED] = Defines.PP_NOT_SAMPLED
#         return False
#     else:
#         headers[Defines.PP_HEADER_PINPOINT_SAMPLED] = Defines.PP_SAMPLED
#     headers[Defines.PP_HEADER_PINPOINT_PAPPTYPE] = pinpoint.get_context(Defines.PP_SERVER_TYPE)
#     headers[Defines.PP_HEADER_PINPOINT_PAPPNAME] = pinpoint.get_context(Defines.PP_APP_NAME)
#     headers['Pinpoint-Flags'] = "0"
#     headers[Defines.PP_HEADER_PINPOINT_HOST] = func
#     headers[Defines.PP_HEADER_PINPOINT_CLIENT] = pinpoint.
#     headers[Defines.PP_HEADER_PINPOINT_TRACEID] = pinpoint.get_context(Defines.PP_TRANSCATION_ID)
#     headers[Defines.PP_HEADER_PINPOINT_PSPANID] = pinpoint.get_context(Defines.PP_SPAN_ID)
#     headers[Defines.PP_HEADER_PINPOINT_SPANID] = pinpoint.get_context(Defines.PP_NEXT_SPAN_ID)
#     return True


def startPinpointByEnviron(environ):
    pinpoint.add_trace_header(Defines.PP_APP_NAME, pinpoint.app_name())
    pinpoint.add_trace_header(Defines.PP_APP_ID, pinpoint.app_id())
    pinpoint.add_context(Defines.PP_APP_NAME, pinpoint.app_name())
    ###############################################################
    # print("------------------- call before -----------------------")
    pinpoint.add_trace_header(Defines.PP_INTERCEPTOR_NAME, 'WSGI handle')

    path, remote_addr, host = environ['PATH_INFO'], environ['REMOTE_ADDR'], environ['HTTP_HOST']

    pinpoint.add_trace_header(Defines.PP_REQ_URI, path)
    pinpoint.add_trace_header(Defines.PP_REQ_CLIENT, remote_addr)
    pinpoint.add_trace_header(Defines.PP_REQ_SERVER, host)
    pinpoint.add_trace_header(Defines.PP_SERVER_TYPE, Defines.PYTHON)
    pinpoint.add_context(Defines.PP_SERVER_TYPE, Defines.PYTHON)
    # nginx add http
    if Defines.PP_HTTP_PINPOINT_PSPANID in environ:
        pinpoint.add_trace_header(Defines.PP_PARENT_SPAN_ID, environ[Defines.PP_HTTP_PINPOINT_PSPANID])

    if Defines.PP_HTTP_PINPOINT_SPANID in environ:
        sid = environ[Defines.PP_HTTP_PINPOINT_SPANID]
    elif Defines.PP_HEADER_PINPOINT_SPANID in environ:
        sid = environ[Defines.PP_HEADER_PINPOINT_SPANID]
    else:
        sid = pinpoint.gen_sid()

    if Defines.PP_HTTP_PINPOINT_TRACEID in environ:
        tid = environ[Defines.PP_HTTP_PINPOINT_TRACEID]
    elif Defines.PP_HEADER_PINPOINT_TRACEID in environ:
        tid = environ[Defines.PP_HEADER_PINPOINT_TRACEID]
    else:
        tid = pinpoint.gen_tid()

    if Defines.PP_HTTP_PINPOINT_PAPPNAME in environ:
        pname = environ[Defines.PP_HTTP_PINPOINT_PAPPNAME]
        pinpoint.add_context(Defines.PP_PARENT_NAME, pname)
        pinpoint.add_trace_header(Defines.PP_PARENT_NAME, pname)

    if Defines.PP_HTTP_PINPOINT_PAPPTYPE in environ:
        ptype = environ[Defines.PP_HTTP_PINPOINT_PAPPTYPE]
        pinpoint.add_context(Defines.PP_PARENT_TYPE, ptype)
        pinpoint.add_trace_header(Defines.PP_PARENT_TYPE, ptype)

    if Defines.PP_HTTP_PINPOINT_HOST in environ:
        Ah = environ[Defines.PP_HTTP_PINPOINT_HOST]
        pinpoint.add_context(Defines.PP_PARENT_HOST, Ah)
        pinpoint.add_trace_header(Defines.PP_PARENT_HOST, Ah)

    # Not nginx, no http
    if Defines.PP_HEADER_PINPOINT_PSPANID in environ:
        pinpoint.add_trace_header(Defines.PP_PARENT_SPAN_ID, environ[Defines.PP_HEADER_PINPOINT_PSPANID])
        # print("PINPOINT_PSPANID:", environ[PP_HEADER_PINPOINT_PSPANID])

    if Defines.PP_HEADER_PINPOINT_PAPPNAME in environ:
        pname = environ[Defines.PP_HEADER_PINPOINT_PAPPNAME]
        pinpoint.add_context(Defines.PP_PARENT_NAME, pname)
        pinpoint.add_trace_header(Defines.PP_PARENT_NAME, pname)

    if Defines.PP_HEADER_PINPOINT_PAPPTYPE in environ:
        ptype = environ[Defines.PP_HEADER_PINPOINT_PAPPTYPE]
        pinpoint.add_context(Defines.PP_PARENT_TYPE, ptype)
        pinpoint.add_trace_header(Defines.PP_PARENT_TYPE, ptype)

    if Defines.PP_HEADER_PINPOINT_HOST in environ:
        Ah = environ[Defines.PP_HEADER_PINPOINT_HOST]
        pinpoint.add_context(Defines.PP_PARENT_HOST, Ah)
        pinpoint.add_trace_header(Defines.PP_PARENT_HOST, Ah)

    if Defines.PP_NGINX_PROXY in environ:
        pinpoint.add_trace_header(Defines.PP_NGINX_PROXY, environ[Defines.PP_NGINX_PROXY])

    if Defines.PP_APACHE_PROXY in environ:
        pinpoint.add_trace_header(Defines.PP_APACHE_PROXY, environ[Defines.PP_APACHE_PROXY])

    pinpoint.add_context(Defines.PP_HEADER_PINPOINT_SAMPLED, "s1")
    if (Defines.PP_HTTP_PINPOINT_SAMPLED in environ and environ[
        Defines.PP_HTTP_PINPOINT_SAMPLED] == Defines.PP_NOT_SAMPLED) or pinpoint.check_trace_limit():
        pinpoint.drop_trace()
        pinpoint.add_context(Defines.PP_HEADER_PINPOINT_SAMPLED, "s0")

    pinpoint.add_trace_header(Defines.PP_TRANSCATION_ID, tid)
    pinpoint.add_trace_header(Defines.PP_SPAN_ID, sid)
    pinpoint.add_context(Defines.PP_TRANSCATION_ID, tid)
    pinpoint.add_context(Defines.PP_SPAN_ID, sid)


def endPinpointByEnviron(ret):
    # for future use
    pass
