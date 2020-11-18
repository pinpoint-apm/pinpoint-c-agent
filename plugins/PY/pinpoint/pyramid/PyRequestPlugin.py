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

# Created by eeliu at 11/12/20


from pinpoint.settings import *
from pinpoint.common import *

class PyRequestPlugin(RequestPlugin):
    def onBefore(self,*args, **kwargs):
        super().onBefore(*args, **kwargs)
        request = args[0]
        pinpointPy.add_clue(PP_INTERCEPTER_NAME, 'Pyramid-middleware')
        pinpointPy.add_clue(PP_REQ_URI, request.path)
        pinpointPy.add_clue(PP_REQ_CLIENT, request.remote_addr)
        pinpointPy.add_clue(PP_REQ_SERVER, request.host)


    def onEnd(self, ret):
        response  = ret
        return super().onEnd(response)
