#!/usr/bin/env python
# -*- coding: UTF-8 -*-


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


from pinpointPy.Fastapi import AsyCommon
from pinpointPy import pinpoint, Defines


class AioRedisPlugins(AsyCommon.AsyncPinTrace):

    def __init__(self, name):
        super().__init__(name)

    def onBefore(self, parentId, *args, **kwargs):
        traceId, args, kwargs = super().onBefore(parentId, *args, **kwargs)
        ###############################################################
        pinpoint.add_trace_header(
            Defines.PP_INTERCEPTOR_NAME, self.getUniqueName(), traceId)
        pinpoint.add_trace_header(
            Defines.PP_SERVER_TYPE, Defines.PP_REDIS, traceId)
        dst = (str(args[0]).split("<")[3]).strip(">")
        pinpoint.add_trace_header(Defines.PP_DESTINATION, dst, traceId)
        pinpoint.add_trace_header_v2(Defines.PP_RETURN, str(args[1:]), traceId)
        ###############################################################
        return traceId, args, kwargs

    def onEnd(self, traceId, ret):
        ###############################################################
        # add_trace_header_v2(PP_RETURN,str(ret))
        ###############################################################
        super().onEnd(traceId, ret)
        return ret

    def onException(self, traceId, e):
        pinpoint.add_trace_header(Defines.PP_ADD_EXCEPTION, str(e), traceId)
