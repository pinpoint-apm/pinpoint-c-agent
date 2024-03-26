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
# Created by eeliu at 3/25/24
import grpc
from grpc_interceptor import ServerInterceptor
from typing import Callable, Any
from pinpointPy import Common, pinpoint, Defines


class GrpcRequestPlugins(Common.PinTrace):
    def __init__(self, name):
        super().__init__(name)

    @staticmethod
    def isSample(*args, **kwargs):
        return True, 0, args, kwargs

    def onBefore(self, parentId, *args, **kwargs):
        trace_id, args, kwargs = super().onBefore(parentId, *args, **kwargs)
        pinpoint.add_trace_header(
            Defines.PP_APP_NAME, pinpoint.app_name(), trace_id)
        pinpoint.add_trace_header(
            Defines.PP_APP_ID, pinpoint.app_id(), trace_id)
        pinpoint.add_context(Defines.PP_APP_NAME,
                             pinpoint.app_name(), trace_id)
        pinpoint.add_trace_header(
            Defines.PP_INTERCEPTOR_NAME, self.getUniqueName(), trace_id)
        context = args[3]
        method = args[4]
        pinpoint.add_trace_header(
            Defines.PP_REQ_CLIENT, context.peer(), trace_id, trace_id)
        pinpoint.add_trace_header(Defines.PP_REQ_URI, method, trace_id)
        pinpoint.add_trace_header(
            Defines.PP_REQ_SERVER, "not_support", trace_id)
        pinpoint.add_trace_header(
            Defines.PP_SERVER_TYPE, Defines.PYTHON, trace_id)
        pinpoint.add_context(Defines.PP_SERVER_TYPE, Defines.PYTHON, trace_id)
        self.sid = pinpoint.gen_sid()
        pinpoint.add_context(Defines.PP_SPAN_ID, self.sid, trace_id)
        self.tid = pinpoint.gen_tid()
        pinpoint.add_context(Defines.PP_TRANSCATION_ID, self.tid, trace_id)
        pinpoint.add_trace_header(
            Defines.PP_TRANSCATION_ID, self.tid, trace_id)
        pinpoint.add_trace_header(Defines.PP_SPAN_ID, self.sid, trace_id)
        pinpoint.add_context(Defines.PP_TRANSCATION_ID, self.tid, trace_id)
        pinpoint.add_context(Defines.PP_SPAN_ID, self.sid, trace_id)
        return trace_id, args, kwargs

    def onEnd(self, trace_id, ret):
        super().onEnd(trace_id, ret)
        return ret

    def onException(self, trace_id, e):
        pinpoint.mark_as_error(str(e), "", trace_id)
        raise e


class PinpointServerInterceptor(ServerInterceptor):
    @GrpcRequestPlugins("grpc.intercept")
    def intercept(
        self,
        method: Callable,
        request_or_iterator: Any,
        context: grpc.ServicerContext,
        method_name: str,
    ) -> Any:
        ret = super().intercept(method, request_or_iterator, context, method_name)
        return ret
