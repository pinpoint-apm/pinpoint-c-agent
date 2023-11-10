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


from pinpointPy.Fastapi.AsyCommon import AsyncPinTrace
from pinpointPy import Defines, pinpoint


class CommonPlugin(AsyncPinTrace):

    # -> tuple[int, Any, dict[str, Any]]:
    def onBefore(self, parentId, *args, **kwargs):
        traceId, args, kwargs = super().onBefore(parentId, *args, **kwargs)
        ###############################################################
        pinpoint.add_trace_header(
            Defines.PP_INTERCEPTOR_NAME, self.getUniqueName(), traceId)
        pinpoint.add_trace_header(
            Defines.PP_SERVER_TYPE, Defines.PP_REMOTE_METHOD, traceId)
        arg = self.get_arg(*args, **kwargs)
        pinpoint.add_trace_header_v2(Defines.PP_ARGS, arg, traceId)
        ###############################################################
        return traceId, args, kwargs

    def onEnd(self, traceId, ret):
        ###############################################################
        pinpoint.add_trace_header_v2(Defines.PP_RETURN, str(ret), traceId)
        ###############################################################
        super().onEnd(traceId, ret)

    def onException(self, traceId, e):
        pinpoint.add_trace_header(Defines.PP_ADD_EXCEPTION, str(e), traceId)

    def get_arg(self, *args, **kwargs):
        args_tmp = {}
        j = 0

        for i in args:
            args_tmp["arg[" + str(j) + "]"] = (str(i))
            j += 1

        for k in kwargs:
            args_tmp[k] = kwargs[k]

        return str(args_tmp)
