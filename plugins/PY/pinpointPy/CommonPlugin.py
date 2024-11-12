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


from pinpointPy import Common, Defines, pinpoint
import random


class PinpointCommonPlugin(Common.PinTrace):

    def onBefore(self, parentId, *args, **kwargs):
        trace_id, args, kwargs = super().onBefore(parentId, *args, **kwargs)
        pinpoint.add_trace_header(
            Defines.PP_INTERCEPTOR_NAME, self.getUniqueName(), trace_id)
        pinpoint.add_trace_header(
            Defines.PP_SERVER_TYPE, Defines.PP_METHOD_CALL, trace_id)
        return trace_id, args, kwargs

    def onEnd(self, traceId, ret):
        # pinpoint.add_trace_header_v2(Defines.PP_RETURN, str(ret), traceId)
        super().onEnd(traceId, ret)
        return ret

    def onException(self, traceId, e):
        pinpoint.add_exception(str(e), traceId)
        raise e


class AsyncCommonPlugin(Common.AsyncPinTrace):

    # -> tuple[int, Any, dict[str, Any]]:
    def onBefore(self, parentId, *args, **kwargs):
        traceId, args, kwargs = super().onBefore(parentId, *args, **kwargs)
        pinpoint.add_trace_header(
            Defines.PP_INTERCEPTOR_NAME, self.getUniqueName(), traceId)
        pinpoint.add_trace_header(
            Defines.PP_SERVER_TYPE, Defines.PP_METHOD_CALL, traceId)
        return traceId, args, kwargs

    def onEnd(self, traceId, ret):
        super().onEnd(traceId, ret)

    def onException(self, traceId, e):
        pinpoint.add_exception(str(e), traceId)


class HookTargetPlugins(Common.PinTrace):
    def onBefore(self, parentId: int,  *args, **kwargs):

        traceId, args, kwargs = super().onBefore(parentId, *args, **kwargs)
        pinpoint.add_trace_header(
            Defines.PP_INTERCEPTOR_NAME, self.getUniqueName(), traceId)
        pinpoint.add_trace_header(
            Defines.PP_SERVER_TYPE, Defines.P_INVOCATION_CALL_TYPE, traceId)

        async_id = random.randint(0, 9999)
        pinpoint.add_trace_header(
            Defines.PP_ASYNC_CALL_ID, f'{async_id}', traceId)

        sequence_id = pinpoint.get_sequence_id(traceId)

        tid = pinpoint.get_context(Defines.PP_TRANSACTION_ID, traceId)
        seq_id = pinpoint.get_context(Defines.PP_SPAN_ID, traceId)
        app_name = pinpoint.get_context(Defines.PP_APP_NAME, traceId)
        app_id = pinpoint.get_context(Defines.PP_APP_ID, traceId)

        if 'target' in kwargs:
            origin_target = kwargs['target']

            def pp_new_entry_func(*args, **kwargs):
                # start trace
                thread_trace_id = pinpoint.with_trace(0)
                self.setCurrentTraceNodeId(thread_trace_id)
                pinpoint.add_trace_header(
                    Defines.PP_APP_NAME, app_name, thread_trace_id)
                pinpoint.add_context(
                    Defines.PP_APP_NAME, app_name, thread_trace_id)

                pinpoint.add_trace_header(
                    Defines.PP_APP_ID, app_id, thread_trace_id)
                pinpoint.add_context(
                    Defines.PP_APP_ID, app_id, thread_trace_id)

                pinpoint.add_trace_header(
                    Defines.PP_SPAN_ID, seq_id, thread_trace_id)
                pinpoint.add_context(
                    Defines.PP_SPAN_ID, seq_id, thread_trace_id)

                pinpoint.add_trace_header(
                    Defines.PP_TRANSACTION_ID, tid, thread_trace_id)
                pinpoint.add_context(
                    Defines.PP_TRANSACTION_ID, tid, thread_trace_id)

                pinpoint.add_trace_header(
                    Defines.PP_SERVER_TYPE, Defines.PYTHON, thread_trace_id)
                pinpoint.set_async_context(
                    thread_trace_id, async_id, sequence_id)

                if callable(origin_target):
                    @PinpointCommonPlugin(origin_target.__name__)
                    def call_origin_target(*args, **kwargs):
                        origin_target(*args, **kwargs)

                    call_origin_target(*args, **kwargs)

                pinpoint.end_trace(thread_trace_id)

            kwargs['target'] = pp_new_entry_func

        return traceId, args, kwargs

    def onEnd(self, traceId, ret):
        super().onEnd(traceId, ret)
        return ret
