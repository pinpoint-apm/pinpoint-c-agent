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


from .AsyRequestPlugin import *
from .. import pinpoint
from .. import Defines

class FastAPIRequestPlugin(AsyRequestPlugin):
    def onBefore(self,*args, **kwargs):
        super().onBefore(*args, **kwargs)
        request = args[0].scope
        pinpoint.add_trace_header(Defines.PP_INTERCEPTOR_NAME, 'fastapi-middleware',self.traceId)
        pinpoint.add_trace_header(Defines.PP_REQ_URI, request["path"], self.traceId)
        pinpoint.add_trace_header(Defines.PP_REQ_CLIENT, request["client"][0], self.traceId)
        pinpoint.add_trace_header(Defines.PP_REQ_SERVER, request["server"][0] + ":" + str(request["server"][1]), self.traceId)


    def onEnd(self, ret):
        response  = ret
        return super().onEnd(response)

