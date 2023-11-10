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


from pinpointPy import pinpoint
from pinpointPy import Defines
from pinpointPy.RequestPlugins import RequestPlugin


class PyRequestPlugin(RequestPlugin):
    # -> tuple[int, tuple[Any, ...], dict[str, Any]]:
    def onBefore(self, parentId, *args, **kwargs):
        trace_id, _, _ = super().onBefore(parentId, *args, **kwargs)
        request = args[0]
        pinpoint.add_trace_header(
            Defines.PP_INTERCEPTOR_NAME, 'Pyramid-middleware', trace_id)
        pinpoint.add_trace_header(Defines.PP_REQ_URI, request.path, trace_id)
        pinpoint.add_trace_header(
            Defines.PP_REQ_CLIENT, request.remote_addr, trace_id)
        pinpoint.add_trace_header(
            Defines.PP_REQ_SERVER, request.host, trace_id)
        return trace_id, args, kwargs

    def onEnd(self, trace_id, ret):
        return super().onEnd(trace_id, ret)
