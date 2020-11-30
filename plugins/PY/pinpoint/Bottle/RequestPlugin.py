#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 11/9/20
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

from bottle import response

from pinpoint.libs import monkey_patch_for_pinpoint
monkey_patch_for_pinpoint()

import pinpointPy
from pinpoint.common import *

class RequestPlugin(WSGIPlugin):

    def onEnd(self, ret):
        pinpointPy.add_clues(PP_HTTP_STATUS_CODE,str(response.status_code))
        if response.status_code !=200:
            pinpointPy.add_clue(PP_ADD_EXCEPTION,"status code not 200")
        return super().onEnd(ret)