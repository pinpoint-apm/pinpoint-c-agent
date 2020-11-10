#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 8/20/20

from urllib.parse import urlparse

from ...settings import *
from .PinpointDefine import *
import random
import pinpointPy

tid = None
nsid = None
sid = None

def generateTid():
    global tid
    tid = ('%s^%s^%s') % (APP_ID, APP_SUB_ID, str(pinpointPy.unique_id()))
    return tid

def generateSid():
    global sid
    sid = str(random.randint(0, 2147483647))
    return sid

def generateNextSid():
    global nsid
    nsid = str(random.randint(0, 2147483647))
    return nsid

def generatePinpointHeader(url, headers):

    if pinpointPy.check_tracelimit():
        headers[PP_HTTP_SAMPLED] = PP_NOT_SAMPLED
        return
    else:
        headers[PP_HTTP_SAMPLED] = PP_SAMPLED
    headers[PP_HEADER_PINPOINT_PAPPTYPE] = PYTHON
    headers[PP_HEADER_PINPOINT_PAPPNAME] = APP_NAME
    headers['Pinpoint-Flags'] = "0"
    headers[PP_HEADER_PINPOINT_HOST] =  urlparse(url)['netloc']
    headers[PP_HEADER_PINPOINT_TRACEID] = pinpointPy.get_context_key(PP_TRANSCATION_ID)
    headers[PP_HEADER_PINPOINT_PSPANID] = pinpointPy.get_context_key(PP_SPAN_ID)
    nsid = generateSid()
    pinpointPy.set_context_key(PP_NEXT_SPAN_ID, nsid)
    headers[PP_HEADER_PINPOINT_SPANID] = nsid