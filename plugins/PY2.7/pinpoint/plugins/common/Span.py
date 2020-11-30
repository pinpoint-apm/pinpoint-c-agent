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
    tid = ('%s^%s^%s') % (APP_ID, str(pinpointPy.start_time()), str(pinpointPy.unique_id()))
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
        headers[SAMPLED] = 's0'
        return
    else:
        headers[SAMPLED] = 's1'
    headers[PINPOINT_PAPPTYPE] = '1700'
    headers[PINPOINT_PAPPNAME] = APP_NAME
    headers['Pinpoint-Flags'] = "0"
    headers[PINPOINT_HOST] =  urlparse(url)['netloc']
    headers[PINPOINT_TRACEID] = pinpointPy.get_context_key('tid')
    headers[PINPOINT_PSPANID] = pinpointPy.get_context_key('sid')
    nsid = generateSid()
    pinpointPy.set_context_key('nsid', nsid)
    headers[PINPOINT_SPANID] = nsid