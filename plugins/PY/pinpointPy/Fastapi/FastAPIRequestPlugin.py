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


from pinpointPy.Fastapi.AsyRequestPlugin import AsyRequestPlugin
from pinpointPy import Defines, pinpoint
import sys


class FastAPIRequestPlugin(AsyRequestPlugin):
    def __init__(self, name):
        super().__init__(name)

    def onBefore(self, parentId, *args, **kwargs):
        traceId, args, kwargs = super().onBefore(parentId, *args, **kwargs)
        request = args[0]
        pinpoint.add_trace_header(
            Defines.PP_INTERCEPTOR_NAME, 'fastapi-middleware', traceId)
        pinpoint.add_trace_header(Defines.PP_REQ_URI, request["path"], traceId)
        pinpoint.add_trace_header(
            Defines.PP_REQ_CLIENT, f'{request.client.host}:{request.client.port}', traceId)
        pinpoint.add_trace_header(
            Defines.PP_REQ_SERVER, request.base_url.hostname, traceId)
        self.request = request
        return traceId, args, kwargs

    def onEnd(self, traceId, response):
        ut = self.request.scope['root_path'] + self.request.scope['route'].path
        pinpoint.add_trace_header(Defines.PP_URL_TEMPLATED, ut, traceId)

        if 'unittest' in sys.modules.keys():
            response.headers["UT"] = ut

        return super().onEnd(traceId, response)
