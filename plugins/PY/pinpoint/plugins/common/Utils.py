#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 11/10/20

import pinpointPy
from pinpoint.settings import *
from .PinpointDefine import  *
from .Span import *

def startPinpointByEnviron(environ):
    pinpointPy.add_clue(PP_APP_NAME, APP_NAME)
    pinpointPy.add_clue(PP_APP_ID, APP_ID)
    ###############################################################
    # print("------------------- call before -----------------------")
    pinpointPy.add_clue(PP_INTERCEPTER_NAME, 'BaseFlaskrequest')


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
        print("PINPOINT_PSPANID:", environ[PP_HEADER_PINPOINT_PSPANID])

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

    pinpointPy.set_context_key("Pinpoint-Sampled","s1")
    if (PP_HTTP_SAMPLED in environ and environ[PP_HTTP_SAMPLED] == PP_NOT_SAMPLED) or pinpointPy.check_tracelimit():
        if environ[PP_HTTP_SAMPLED] == PP_NOT_SAMPLED:
            pinpointPy.drop_trace()
            pinpointPy.set_context_key("Pinpoint-Sampled", "s0")


    pinpointPy.add_clue(PP_TRANSCATION_ID,tid)
    pinpointPy.add_clue(PP_SPAN_ID, sid)
    pinpointPy.set_context_key(PP_TRANSCATION_ID, tid)
    pinpointPy.set_context_key(PP_SPAN_ID, sid)

def endPinpointByEnviron(ret):
    # for future use
    pass