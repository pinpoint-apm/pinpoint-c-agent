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


from .AsyCommon import AsynPinTrace
from .. import Defines
from .. import pinpoint


class CommonPlugin(AsynPinTrace):

    def onBefore(self, *args, **kwargs):
        super().onBefore(*args, **kwargs)
        ###############################################################
        pinpoint.add_trace_header(Defines.PP_INTERCEPTOR_NAME, self.getFuncUniqueName(), self.traceId)
        pinpoint.add_trace_header(Defines.PP_SERVER_TYPE, Defines.PP_REMOTE_METHOD, self.traceId)
        arg = self.get_arg(*args, **kwargs)
        pinpoint.add_trace_header_v2(Defines.PP_ARGS, arg, self.traceId)
        ###############################################################
        # print( threading.currentThread().ident)
        return args, kwargs

    def onEnd(self, ret):
        ###############################################################
        pinpoint.add_trace_header_v2(Defines.PP_RETURN, str(ret), self.traceId)
        ###############################################################
        super().onEnd(ret)
        return ret

    def onException(self, e):
        pinpoint.add_trace_header(Defines.PP_ADD_EXCEPTION, str(e), self.traceId)

    def get_arg(self, *args, **kwargs):
        args_tmp = {}
        j = 0

        for i in args:
            args_tmp["arg[" + str(j) + "]"] = (str(i))
            j += 1

        for k in kwargs:
            args_tmp[k] = kwargs[k]

        return str(args_tmp)
