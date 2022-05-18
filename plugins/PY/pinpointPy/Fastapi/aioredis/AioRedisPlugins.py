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


from .. import AsyCommon
from ... import pinpoint
from ... import Defines


class AioRedisPlugins(AsyCommon.AsynPinTrace):

    def __init__(self, name):
        super().__init__(name)
        self.dst = ''

    def onBefore(self, *args, **kwargs):
        super().onBefore(*args, **kwargs)
        ###############################################################
        pinpoint.add_trace_header(Defines.PP_INTERCEPTOR_NAME, self.getFuncUniqueName(), self.traceId)
        pinpoint.add_trace_header(Defines.PP_SERVER_TYPE, Defines.PP_REDIS, self.traceId)
        dst = (str(args[0]).split("<")[3]).strip(">")
        pinpoint.add_trace_header(Defines.PP_DESTINATION, dst, self.traceId)
        pinpoint.add_trace_header_v2(Defines.PP_RETURN, str(args[1:]), self.traceId)
        ###############################################################
        return args, kwargs

    def onEnd(self, ret):
        ###############################################################
        # add_trace_header_v2(PP_RETURN,str(ret))
        ###############################################################
        super().onEnd(ret)
        return ret

    def onException(self, e):
        pinpoint.add_trace_header(Defines.PP_ADD_EXCEPTION, str(e),self.traceId)
