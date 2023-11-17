#!/usr/bin/env python
# -*- coding: UTF-8 -*-

# Created by eeliu at 8/20/20


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


from pinpointPy import Common
from pinpointPy import pinpoint
from pinpointPy import Defines


class PyRedisPlugins(Common.PinTrace):

    def __init__(self, name):
        super().__init__(name)

    def onBefore(self, parentId, *args, **kwargs):
        trace_id, args, kwargs = super().onBefore(parentId, *args, **kwargs)
        pinpoint.add_trace_header(
            Defines.PP_INTERCEPTOR_NAME, self.getUniqueName(), trace_id)
        pinpoint.add_trace_header(
            Defines.PP_SERVER_TYPE, Defines.PP_REDIS, trace_id)
        pinpoint.add_trace_header(
            Defines.PP_DESTINATION, str(args[0]), trace_id)
        return trace_id, args, kwargs

    def onEnd(self, traceId, ret):
        super().onEnd(traceId, ret)
        return ret

    def onException(self, traceId, e):
        pinpoint.add_trace_header(Defines.PP_ADD_EXCEPTION, str(e), traceId)


class PyRedisPipeLinePlugins(PyRedisPlugins):
    def onBefore(self, parentId, *args, **kwargs):
        trace_id, args, kwargs = Common.PinTrace.onBefore(
            self, parentId, *args, **kwargs)
        pinpoint.add_trace_header(
            Defines.PP_INTERCEPTOR_NAME, self.getUniqueName(), trace_id)
        pinpoint.add_trace_header(
            Defines.PP_SERVER_TYPE, Defines.PP_REDIS, trace_id)
        from redis.client import Pipeline
        if isinstance(args[0], Pipeline) and args[0]:
            pipeLine = args[0]
            pinpoint.add_trace_header(
                Defines.PP_DESTINATION, str(pipeLine.connection_pool.get_connection("")), trace_id)
            import sys
            if 'unittest' in sys.modules.keys():
                pipeLine._pinpoint_ = True

        return trace_id, args, kwargs
