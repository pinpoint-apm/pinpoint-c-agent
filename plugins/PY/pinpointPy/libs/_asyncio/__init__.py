#!/usr/bin/env python
# -*- coding: UTF-8 -*-

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

# Created by eeliu at 24/11/04

from pinpointPy.Interceptor import intercept_once, Interceptor
from pinpointPy.Common import PinTrace
from pinpointPy.CommonPlugin import AsyncCommonPlugin
from pinpointPy import pinpoint, Defines, get_logger

import random
import asyncio


class HookAsyncMainTaskPlugin(PinTrace):

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

        main_coroutine = args[0]

        async def pp_new_main_coroutine():
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

            if asyncio.iscoroutine(main_coroutine):
                @AsyncCommonPlugin(main_coroutine.__name__)
                async def call_main_coroutine():
                    return await main_coroutine
                ret = await call_main_coroutine()

            pinpoint.end_trace(thread_trace_id)

            return ret

        # args[0] = pp_new_main_coroutine(args[0])
        return traceId, [pp_new_main_coroutine()], kwargs

    def onEnd(self, traceId, ret):
        super().onEnd(traceId, ret)
        return ret


class HookAsyncMainTaskPlugin_create_task(HookAsyncMainTaskPlugin):
    def getUniqueName(self):
        return "asyncio.create_task"


class HookAsyncMainTaskPlugin_run(HookAsyncMainTaskPlugin):
    def getUniqueName(self):
        return "asyncio.run"


@intercept_once
def monkey_patch():
    try:
        # from asyncio import tasks
        # from asyncio import runners
        import asyncio
        Interceptors = [
            Interceptor(asyncio, 'create_task',
                        HookAsyncMainTaskPlugin_create_task),
            Interceptor(asyncio, 'run', HookAsyncMainTaskPlugin_run)
        ]

        for interceptor in Interceptors:
            interceptor.enable()
    except ImportError as e:
        get_logger().info(f"exception at {e}")


__all__ = ['monkey_patch']

__version__ = '0.0.1'
__author__ = 'liu.mingyi@navercorp.com'
