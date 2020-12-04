#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 12/3/20

# ******************************************************************************
#   Copyright  2020. NAVER Corp.
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
# ******************************************************************************

from pinpoint.common import *
import pinpointPy

class PyMemcachedPlugin(Candy):
    def __init__(self, name):
        super(PyMemcachedPlugin, self).__init__(name)

    def onBefore(self, *args, **kwargs):
        super(PyMemcachedPlugin, self).onBefore(*args, **kwargs)
        pinpointPy.add_clue(PP_INTERCEPTOR_NAME,self.getFuncUniqueName())
        ###############################################################
        client = args[0]
        method = args[1]
        host = client.server
        pinpointPy.add_clue(PP_DESTINATION, host)
        pinpointPy.add_clue(PP_SERVER_TYPE, PP_MEMCACHED_FUTURE_GET)
        pinpointPy.add_clues(PP_ARGS,method)
        ###############################################################

        return args, kwargs

    def onEnd(self, ret):
        super(PyMemcachedPlugin, self).onEnd(ret)
        return ret
