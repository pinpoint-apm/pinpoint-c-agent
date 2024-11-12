# ------------------------------------------------------------------------------
#  Copyright  2024. NAVER Corp.                                                -
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
from ast import arg
from pinpointPy.CommonPlugin import PinpointCommonPlugin, AsyncCommonPlugin, Defines, pinpoint
from pinpointPy.Common import PinTrace
import random

_PP_TID = '_pp_tid_'
_PP_SPAN_ID = '_pp_span_id_'
_PP_NAME = '_pp_app_name_'
_PP_ID = '_pp_app_id_'
_PP_SEQ_ID = '_pp_sequence_id_'
_PP_ASYNC_ID = '_pp_async_id_'

_pinpoint_id_set = (_PP_TID, _PP_SPAN_ID, _PP_NAME,
                    _PP_ID, _PP_SEQ_ID, _PP_ASYNC_ID)


class CeleryWorkerPlugin(PinTrace):

    @staticmethod
    def isSample(*args, **kwargs):
        return True, 0, args, kwargs

    def onBefore(self, parentId, *args, **kwargs):
        traceId, args, kwargs = super().onBefore(parentId, *args, **kwargs)

        if _PP_TID not in kwargs:
            sid = pinpoint.gen_sid()
            pinpoint.add_trace_header(Defines.PP_SPAN_ID, sid, traceId)
            pinpoint.add_context(Defines.PP_SPAN_ID, sid, traceId)

            pinpoint.add_trace_header(
                Defines.PP_INTERCEPTOR_NAME, "celery[worker]", traceId)

            pinpoint.add_trace_header(
                Defines.PP_APP_NAME, pinpoint.app_name(), traceId)
            pinpoint.add_context(
                Defines.PP_APP_NAME, pinpoint.app_name(), traceId)

            pinpoint.add_trace_header(
                Defines.PP_APP_ID, pinpoint.app_id(), traceId)
            pinpoint.add_context(
                Defines.PP_APP_ID, pinpoint.app_id(), traceId)

            pinpoint.add_trace_header(
                Defines.PP_REQ_URI, self.getUniqueName(), traceId)
            pinpoint.add_trace_header(
                Defines.PP_REQ_SERVER, "localhost", traceId)
            pinpoint.add_trace_header(
                Defines.PP_REQ_CLIENT, "localhost", traceId)
            pinpoint.add_trace_header(
                Defines.PP_SERVER_TYPE, Defines.PYTHON, traceId)
            pinpoint.add_context(Defines.PP_SERVER_TYPE,
                                 Defines.PYTHON, traceId)

            pinpoint.add_context(
                Defines.PP_HEADER_PINPOINT_SAMPLED, "s1", traceId)

            pinpoint.add_trace_header(
                Defines.PP_PARENT_TYPE, Defines.PYTHON, traceId)
            pinpoint.add_trace_header(
                Defines.PP_PARENT_NAME, pinpoint.app_name(), traceId)
            pinpoint.add_trace_header(
                Defines.PP_PARENT_HOST, "localhost", traceId)

            tid = pinpoint.gen_tid()

            pinpoint.add_trace_header(Defines.PP_TRANSACTION_ID, tid, traceId)
            pinpoint.add_context(Defines.PP_TRANSACTION_ID, tid, traceId)

            return traceId, args, kwargs
        else:
            tid = kwargs[_PP_TID]
            span_id = kwargs[_PP_SPAN_ID]
            pp_name = kwargs[_PP_NAME]
            pp_id = kwargs[_PP_ID]
            pp_seq_id = kwargs[_PP_SEQ_ID]
            pp_async_id = kwargs[_PP_ASYNC_ID]

            pinpoint.add_trace_header(
                Defines.PP_APP_NAME, pp_name, traceId)
            pinpoint.add_context(
                Defines.PP_APP_NAME, pp_name, traceId)

            pinpoint.add_trace_header(
                Defines.PP_APP_ID, pp_id, traceId)
            pinpoint.add_context(
                Defines.PP_APP_ID, pp_id, traceId)

            pinpoint.add_trace_header(
                Defines.PP_SPAN_ID, span_id, traceId)
            pinpoint.add_context(
                Defines.PP_SPAN_ID, span_id, traceId)

            pinpoint.add_trace_header(
                Defines.PP_TRANSACTION_ID, tid, traceId)

            pinpoint.add_context(
                Defines.PP_TRANSACTION_ID, tid, traceId)

            pinpoint.add_trace_header(
                Defines.PP_SERVER_TYPE, Defines.PYTHON, traceId)

            pinpoint.set_async_context(
                traceId, pp_async_id, pp_seq_id)

            for id in _pinpoint_id_set:
                del kwargs[id]

            return traceId, args, kwargs

    def getUniqueName(self):
        return "celery[worker]."+super().getUniqueName()


class CeleryParasInjection_async_Plugin(PinpointCommonPlugin):
    def genPinpointId(self, traceId, async_id):
        sequence_id = pinpoint.get_sequence_id(traceId)
        kwargs = {}
        kwargs[_PP_TID] = pinpoint.get_context(
            Defines.PP_TRANSACTION_ID, traceId)
        kwargs[_PP_SPAN_ID] = pinpoint.get_context(
            Defines.PP_SPAN_ID, traceId)
        kwargs[_PP_NAME] = pinpoint.get_context(
            Defines.PP_APP_NAME, trace_id=traceId)
        kwargs[_PP_ID] = pinpoint.get_context(
            Defines.PP_APP_ID, traceId)
        kwargs[_PP_SEQ_ID] = sequence_id
        kwargs[_PP_ASYNC_ID] = async_id
        return kwargs

    def onBefore(self, parentId, *args, **kwargs):
        traceId, args, kwargs = super().onBefore(parentId, *args, **kwargs)
        pinpoint.add_trace_header(
            Defines.PP_SERVER_TYPE, Defines.P_INVOCATION_CALL_TYPE, traceId)
        async_id = random.randint(0, 9999)
        pinpoint.add_trace_header(
            Defines.PP_ASYNC_CALL_ID, f'{async_id}', traceId)
        pin_kwargs = self.genPinpointId(traceId, async_id)
        if 'kwargs' in kwargs:
            origin_kwargs = kwargs['kwargs']
            origin_kwargs.update(pin_kwargs)
            return traceId, args, kwargs
        elif 'args' in kwargs:
            kwargs['kwargs'] = pin_kwargs
            return traceId, args, kwargs
        else:
            if len(args) >= 2:
                args[1].update(pin_kwargs)
                return traceId, args, kwargs
            elif len(args) == 1:
                arg_1 = pin_kwargs
                return traceId, args+({},), kwargs
            else:
                arg_1 = pin_kwargs
                return traceId, ([], arg_1), kwargs


class CeleryCallerPlugin:
    def __call__(self, func):
        self.func_name = func.__name__
        if func.apply_async:
            apply_async = CeleryParasInjection_async_Plugin(
                f"celery.caller.{self.func_name}.apply_async")(func.apply_async)
        func.apply_async = apply_async

        return func


# @author:eeliu
