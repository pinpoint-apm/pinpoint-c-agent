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

# Created by eeliu at 7/31/20



import pinpointPy

###############################################################

# user should set below before use
APP_ID ='python-agent' # application id
APP_NAME ='PYTHON-AGNNT' # application name
# COLLECTOR_HOST='unix:/tmp/collector-agent.sock'
COLLECTOR_HOST='tcp:dev-collector:9999'
APP_SUB_ID='1'

###############################################################

pinpointPy.set_agent(collector_host=COLLECTOR_HOST,trace_limit=-1)

# pinpointPy.set_agent(collector_host='Tcp:ip:port',trace_limit=-1)

# def output(msg):
#     print(msg)
#
# pinpointPy.enable_debug(None)

__all__=['APP_ID','APP_NAME','APP_SUB_ID','COLLECTOR_HOST']