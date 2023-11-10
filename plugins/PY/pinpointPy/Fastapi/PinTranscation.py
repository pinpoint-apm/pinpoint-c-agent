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

from pinpointPy.Fastapi.AsyCommon import AsyncPinTrace
from pinpointPy import Defines, pinpoint
from pinpointPy.Common import GenPinHeader
from starlette_context import request_cycle_context


class PinStarlettePlugin:
    """support a new conext
        more detail https://starlette-context.readthedocs.io/en/latest/fastapi.html?highlight=request_cycle_context#usage-with-fastapi
    """

    def __call__(self, func):
        async def pinpointTrace(*args, **kwargs):
            _context = {}
            with request_cycle_context(_context):
                return await func(*args, **kwargs)
        return pinpointTrace


class PinTransaction(AsyncPinTrace):

    def __init__(self, name: str, userGenHeaderCb: GenPinHeader):
        """pinpointPy user entry point

        Example: 

            ```
            from pinpointPy.Fastapi import PinTransaction,PinStarlettePlugin
            @PinStarlettePlugin()
            @PinTransaction('consumer_message_from_kafka_callback', MsgHeader())
            async def consumer_message_from_kafka_callback(msg):
                print(f"Consumed msg: . \n")
                await pass_more()
            ```
        Args:
            name (str): entry points name(showing pinpoint)
            userGenHeaderCb (GenPinHeader): This helps getting header from current function 
        """
        super().__init__(name)
        self.name = name
        self.getHeader = userGenHeaderCb

    @staticmethod
    def isSample(*args, **kwargs):
        return True, 0

    def onBefore(self, parentId, *args, **kwargs):
        traceId, args_n, kwargs_n = super().onBefore(parentId, *args, **kwargs)
        header = self.getHeader.GetHeader(*args_n, **kwargs_n)

        sid = pinpoint.gen_sid()
        pinpoint.add_context(Defines.PP_SPAN_ID, sid, traceId)
        pinpoint.add_trace_header(Defines.PP_SPAN_ID, sid, traceId)

        pinpoint.add_trace_header(
            Defines.PP_INTERCEPTOR_NAME, self.name, traceId)
        pinpoint.add_trace_header(
            Defines.PP_APP_NAME, pinpoint.app_name(), traceId)
        pinpoint.add_context(
            Defines.PP_APP_NAME, pinpoint.app_name(), traceId)
        pinpoint.add_trace_header(
            Defines.PP_APP_ID, pinpoint.app_id(), traceId)

        pinpoint.add_trace_header(Defines.PP_REQ_URI, header.Url, traceId)
        pinpoint.add_trace_header(Defines.PP_REQ_SERVER, header.Host, traceId)
        pinpoint.add_trace_header(
            Defines.PP_REQ_CLIENT, header.RemoteAddr, traceId)
        pinpoint.add_trace_header(
            Defines.PP_SERVER_TYPE, Defines.PYTHON, traceId)
        pinpoint.add_context(Defines.PP_SERVER_TYPE, Defines.PYTHON, traceId)
        pinpoint.add_context(Defines.PP_HEADER_PINPOINT_SAMPLED, "s1", traceId)

        if header.ParentType != '':
            pinpoint.add_trace_header(
                Defines.PP_PARENT_TYPE, header.ParentType, traceId)

        if header.ParentName != '':
            pinpoint.add_trace_header(
                Defines.PP_PARENT_NAME, header.ParentName, traceId)

        if header.ParentHost != '':
            pinpoint.add_trace_header(
                Defines.PP_PARENT_HOST, header.ParentHost, traceId)

        tid = ''
        if header.ParentTid != '':
            tid = header.ParentTid
            pinpoint.add_trace_header(Defines.PP_PARENT_SPAN_ID, tid, traceId)
            # nsid = pinpoint.gen_sid()
            pinpoint.add_trace_header(Defines.PP_NEXT_SPAN_ID, sid, traceId)
        else:
            tid = pinpoint.gen_tid()

        pinpoint.add_trace_header(Defines.PP_TRANSCATION_ID, tid, traceId)
        pinpoint.add_context(Defines.PP_TRANSCATION_ID, tid, traceId)

        if header.Error:
            pinpoint.mark_as_error(header.Error, header.Error, 0, traceId)

        return traceId, args_n, kwargs_n

    def onEnd(self, parentId, ret):
        super().onEnd(parentId, ret)

    def onException(self, traceId, e):
        pinpoint.mark_as_error(str(e), "", 0, traceId)
        raise e
