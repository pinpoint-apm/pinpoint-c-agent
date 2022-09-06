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


from os import stat
from pickle import FALSE
from random import sample
from .. import AsyCommon
from ... import pinpoint
from ... import Defines
from ... import Helper

from urllib.parse import urlparse

class HttpxRequestPlugins(AsyCommon.AsynPinTrace):

    def __init__(self, name):
        super().__init__(name)

    @staticmethod
    def isSample(*args, **kwargs):
        '''
        if not root, no trace
        :return:
        '''
        sampled,parentId= AsyCommon.AsynPinTrace.isSample(*args,**kwargs)
        if not sampled:
            return False,None
    
        url = args[0][2]
        target = urlparse(url).netloc
        if "headers" not in kwargs or not kwargs['headers']:
            kwargs["headers"] = {}

        if pinpoint.get_context(Defines.PP_HEADER_PINPOINT_SAMPLED,parentId) == "s1":
            Helper.generatePinpointHeader(target, kwargs['headers'],parentId)
            return True,parentId
        else:
            kwargs['headers'][Defines.PP_HEADER_PINPOINT_SAMPLED] = Defines.PP_NOT_SAMPLED
            return False ,None

    def onBefore(self,parentId, *args, **kwargs):
        url = args[2]
        target = urlparse(url).netloc
        traceId,args,kwargs = super().onBefore(parentId,*args, **kwargs)
        ###############################################################
        pinpoint.add_trace_header(Defines.PP_INTERCEPTOR_NAME, self.getFuncUniqueName(),traceId)
        pinpoint.add_trace_header(Defines.PP_SERVER_TYPE, Defines.PP_REMOTE_METHOD,traceId)
        pinpoint.add_trace_header_v2(Defines.PP_ARGS, url,traceId)
        pinpoint.add_trace_header_v2(Defines.PP_HTTP_URL, url,traceId)
        pinpoint.add_trace_header(Defines.PP_DESTINATION, target,traceId)
        ###############################################################
        return traceId,args, kwargs

    def onEnd(self,traceId, ret):
        ###############################################################
        pinpoint.add_trace_header(Defines.PP_NEXT_SPAN_ID, pinpoint.get_context(Defines.PP_NEXT_SPAN_ID,traceId),traceId)
        pinpoint.add_trace_header_v2(Defines.PP_HTTP_STATUS_CODE, str(ret.status_code),traceId)
        pinpoint.add_trace_header_v2(Defines.PP_RETURN, str(ret),traceId)
        ###############################################################
        super().onEnd(traceId,ret)
        return ret

    def onException(self,traceId, e):
        pinpoint.add_trace_header(Defines.PP_ADD_EXCEPTION, str(e),traceId)

    def get_arg(self, *args, **kwargs):
        args_tmp = {}
        j = 0

        for i in args:
            args_tmp["arg[" + str(j) + "]"] = (str(i))
            j += 1

        for k in kwargs:
            args_tmp[k] = kwargs[k]

        return str(args_tmp)