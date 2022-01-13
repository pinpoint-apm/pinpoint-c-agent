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

# Created by eeliu at 8/20/20

from urllib.parse import urlparse
from .Defines import *
from .Common import local_host_name,local_ip

import random
import _pinpointPy

tid = None
nsid = None
sid = None

def generateTid():
    global tid
    tid = ('%s^%s^%s') % (APP_ID, str(_pinpointPy.start_time()), str(_pinpointPy.unique_id()))
    return tid

def generateSid():
    global sid
    sid = str(random.randint(0, 2147483647))
    return sid

def generateNextSid():
    global nsid
    nsid = str(random.randint(0, 2147483647))
    return nsid

def generatePinpointHeader(host,headers):
    headers[PP_HEADER_PINPOINT_SAMPLED] = PP_SAMPLED
    headers[PP_HEADER_PINPOINT_PAPPTYPE] = _pinpointPy.get_context_key(PP_SERVER_TYPE)
    headers[PP_HEADER_PINPOINT_PAPPNAME] = _pinpointPy.get_context_key(PP_APP_NAME)
    headers['Pinpoint-Flags'] = "0"
    headers[PP_HEADER_PINPOINT_HOST] =host
    headers[PP_HEADER_PINPOINT_TRACEID] = _pinpointPy.get_context_key(PP_TRANSCATION_ID)
    headers[PP_HEADER_PINPOINT_PSPANID] = _pinpointPy.get_context_key(PP_SPAN_ID)
    nsid = generateSid()
    _pinpointPy.set_context_key(PP_NEXT_SPAN_ID, nsid)
    headers[PP_HEADER_PINPOINT_SPANID] = nsid

def generatePPRabbitMqHeader(func,headers):

    if _pinpointPy.get_context_key(PP_HEADER_PINPOINT_SAMPLED) == 's0':
        headers[PP_HEADER_PINPOINT_SAMPLED] = PP_NOT_SAMPLED
        return False
    else:
        headers[PP_HEADER_PINPOINT_SAMPLED] = PP_SAMPLED
    headers[PP_HEADER_PINPOINT_PAPPTYPE] = _pinpointPy.get_context_key(PP_SERVER_TYPE)
    headers[PP_HEADER_PINPOINT_PAPPNAME] = _pinpointPy.get_context_key(PP_APP_NAME)
    headers['Pinpoint-Flags'] = "0"
    headers[PP_HEADER_PINPOINT_HOST] = func
    headers[PP_HEADER_PINPOINT_CLIENT] = local_ip
    headers[PP_HEADER_PINPOINT_TRACEID] = _pinpointPy.get_context_key(PP_TRANSCATION_ID)
    headers[PP_HEADER_PINPOINT_PSPANID] = _pinpointPy.get_context_key(PP_SPAN_ID)
    headers[PP_HEADER_PINPOINT_SPANID] = _pinpointPy.get_context_key(PP_NEXT_SPAN_ID)
    return True