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
        print(args)
        print(kwargs)
        traceId, args, kwargs = super().onBefore(parentId, *args, **kwargs)

        if _PP_TID not in kwargs:
            # todo call common transaction id
            print("--------------------------------------------")
            return traceId, args, kwargs

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
            Defines.PP_TRANSCATION_ID, tid, traceId)

        pinpoint.add_context(
            Defines.PP_TRANSCATION_ID, tid, traceId)

        pinpoint.add_trace_header(
            Defines.PP_SERVER_TYPE, Defines.PYTHON, traceId)

        pinpoint.set_async_context(
            traceId, pp_async_id, pp_seq_id)

        for id in _pinpoint_id_set:
            del kwargs[id]

        return traceId, args, kwargs


class CeleryParasInjection_async_Plugin(PinpointCommonPlugin):
    def genPinpointId(self, traceId, async_id):
        sequence_id = pinpoint.get_sequence_id(traceId)
        kwargs = {}
        kwargs[_PP_TID] = pinpoint.get_context(
            Defines.PP_TRANSCATION_ID, traceId)
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
        print(args)
        print(kwargs)
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
                print(args)
                return traceId, args, kwargs
            elif len(args) == 1:
                arg_1 = pin_kwargs
                # return traceId, args+(arg_1,), kwargs
                print(args+({},))
                return traceId, args+({},), kwargs
            else:
                arg_1 = pin_kwargs
                print(([], arg_1))
                return traceId, ([], arg_1), kwargs

            # print(args)
            # args[1]['tesyt'] = 23434
            # print(kwargs)
            # return traceId, args, kwargs

            # if len(args) >= 2:
            #     args[1].update(pin_kwargs)
            #     print(args)
            #     return traceId, args, kwargs
            # elif len(args) == 1:
            #     arg_1 = pin_kwargs
            #     # return traceId, args+(arg_1,), kwargs
            #     print(args+({},))
            #     return traceId, args+({},), kwargs
            # else:
            #     arg_1 = pin_kwargs
            #     print(([], arg_1))
            #     return traceId, ([], arg_1), kwargs

            # def updateKwargs(self, origin_kwargs, pin_kwargs):
            #     if origin_kwargs:
            #         origin_kwargs.update(pin_kwargs)
            #         return origin_kwargs
            #     else:
            #         return pin_kwargs

            # class CeleryParasInjection_delay_Plugin(CeleryParasInjection_async_Plugin):
            #     def updateKwargs(self, kwargs, pin_kwargs):
            #         if kwargs:
            #             for k, v in pin_kwargs.items():
            #                 kwargs[k] = v
            #             return kwargs
            #         else:
            #             return pin_kwargs


class CeleryCallerPlugin:
    def __call__(self, func):
        self.func_name = func.__name__
        if func.apply_async:
            apply_async = CeleryParasInjection_async_Plugin(
                f"{self.func_name}.apply_async")(func.apply_async)
        # if func.delay:
        #     delay = CeleryParasInjection_delay_Plugin(
        #         f"{self.func_name}.delay")(func.delay)
        func.apply_async = apply_async
        # func.delay = delay

        return func


# @author:eeliu
