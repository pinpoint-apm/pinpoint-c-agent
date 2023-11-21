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
from pinpointPy import Defines, pinpoint, get_logger


def generateNextSid():
    return pinpoint.gen_sid()


def generatePinpointHeader(host, headers, traceId=-1):
    headers[Defines.PP_HEADER_PINPOINT_SAMPLED] = Defines.PP_SAMPLED
    headers[Defines.PP_HEADER_PINPOINT_PAPPTYPE] = pinpoint.get_context(
        Defines.PP_SERVER_TYPE, traceId)
    headers[Defines.PP_HEADER_PINPOINT_PAPPNAME] = pinpoint.get_context(
        Defines.PP_APP_NAME, traceId)
    headers['Pinpoint-Flags'] = "0"
    headers[Defines.PP_HEADER_PINPOINT_HOST] = host
    headers[Defines.PP_HEADER_PINPOINT_TRACEID] = pinpoint.get_context(
        Defines.PP_TRANSCATION_ID, traceId)
    headers[Defines.PP_HEADER_PINPOINT_PSPANID] = pinpoint.get_context(
        Defines.PP_SPAN_ID, traceId)
    nextSeqId = pinpoint.gen_sid()
    pinpoint.add_context(Defines.PP_NEXT_SPAN_ID, nextSeqId, traceId)
    headers[Defines.PP_HEADER_PINPOINT_SPANID] = nextSeqId
    get_logger().debug(f'append PinpointHeader header:{headers}')


def startPinpointByEnviron(environ, trace_id: int):
    pinpoint.add_trace_header(
        Defines.PP_APP_NAME, pinpoint.app_name(), trace_id)
    pinpoint.add_context(Defines.PP_APP_NAME, pinpoint.app_name(), trace_id)
    pinpoint.add_trace_header(Defines.PP_APP_ID, pinpoint.app_id(), trace_id)
    ###############################################################
    pinpoint.add_trace_header(
        Defines.PP_INTERCEPTOR_NAME, 'WSGI handle', trace_id)

    path, remote_addr, host = environ['PATH_INFO'], environ['REMOTE_ADDR'], environ['HTTP_HOST']

    pinpoint.add_trace_header(Defines.PP_REQ_URI, path, trace_id)
    pinpoint.add_trace_header(Defines.PP_REQ_CLIENT, remote_addr, trace_id)
    pinpoint.add_trace_header(Defines.PP_REQ_SERVER, host, trace_id)
    pinpoint.add_trace_header(Defines.PP_SERVER_TYPE, Defines.PYTHON, trace_id)
    pinpoint.add_context(Defines.PP_SERVER_TYPE, Defines.PYTHON, trace_id)
    # nginx add http
    if Defines.PP_HTTP_PINPOINT_PSPANID in environ:
        pinpoint.add_trace_header(
            Defines.PP_PARENT_SPAN_ID, environ[Defines.PP_HTTP_PINPOINT_PSPANID], trace_id)

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
        pinpoint.add_context(Defines.PP_PARENT_NAME, pname, trace_id)
        pinpoint.add_trace_header(Defines.PP_PARENT_NAME, pname, trace_id)

    if Defines.PP_HTTP_PINPOINT_PAPPTYPE in environ:
        ptype = environ[Defines.PP_HTTP_PINPOINT_PAPPTYPE]
        pinpoint.add_context(Defines.PP_PARENT_TYPE, ptype, trace_id)
        pinpoint.add_trace_header(Defines.PP_PARENT_TYPE, ptype, trace_id)

    if Defines.PP_HTTP_PINPOINT_HOST in environ:
        Ah = environ[Defines.PP_HTTP_PINPOINT_HOST]
        pinpoint.add_context(Defines.PP_PARENT_HOST, Ah, trace_id)
        pinpoint.add_trace_header(Defines.PP_PARENT_HOST, Ah, trace_id)

    # Not nginx, no http
    if Defines.PP_HEADER_PINPOINT_PSPANID in environ:
        pinpoint.add_trace_header(
            Defines.PP_PARENT_SPAN_ID, environ[Defines.PP_HEADER_PINPOINT_PSPANID], trace_id)

    if Defines.PP_HEADER_PINPOINT_PAPPNAME in environ:
        pname = environ[Defines.PP_HEADER_PINPOINT_PAPPNAME]
        pinpoint.add_context(Defines.PP_PARENT_NAME, pname, trace_id)
        pinpoint.add_trace_header(Defines.PP_PARENT_NAME, pname, trace_id)

    if Defines.PP_HEADER_PINPOINT_PAPPTYPE in environ:
        ptype = environ[Defines.PP_HEADER_PINPOINT_PAPPTYPE]
        pinpoint.add_context(Defines.PP_PARENT_TYPE, ptype, trace_id)
        pinpoint.add_trace_header(Defines.PP_PARENT_TYPE, ptype, trace_id)

    if Defines.PP_HEADER_PINPOINT_HOST in environ:
        Ah = environ[Defines.PP_HEADER_PINPOINT_HOST]
        pinpoint.add_context(Defines.PP_PARENT_HOST, Ah, trace_id)
        pinpoint.add_trace_header(Defines.PP_PARENT_HOST, Ah, trace_id)

    if Defines.PP_NGINX_PROXY in environ:
        pinpoint.add_trace_header(
            Defines.PP_NGINX_PROXY, environ[Defines.PP_NGINX_PROXY], trace_id)

    if Defines.PP_APACHE_PROXY in environ:
        pinpoint.add_trace_header(
            Defines.PP_APACHE_PROXY, environ[Defines.PP_APACHE_PROXY], trace_id)

    pinpoint.add_context(Defines.PP_HEADER_PINPOINT_SAMPLED, "s1", trace_id)
    if (Defines.PP_HTTP_PINPOINT_SAMPLED in environ and environ[
            Defines.PP_HTTP_PINPOINT_SAMPLED] == Defines.PP_NOT_SAMPLED) or pinpoint.check_trace_limit():
        pinpoint.drop_trace(trace_id)
        pinpoint.add_context(
            Defines.PP_HEADER_PINPOINT_SAMPLED, "s0", trace_id)

    pinpoint.add_trace_header(Defines.PP_TRANSCATION_ID, tid, trace_id)
    pinpoint.add_context(Defines.PP_TRANSCATION_ID, tid, trace_id)

    pinpoint.add_trace_header(Defines.PP_SPAN_ID, sid, trace_id)
    pinpoint.add_context(Defines.PP_SPAN_ID, sid, trace_id)


def endPinpointByEnviron(ret, trace_id: int):
    # for future use
    pass
