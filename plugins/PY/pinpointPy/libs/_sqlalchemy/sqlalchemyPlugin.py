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

# Created by eeliu at 7/31/20


from pinpointPy import pinpoint, Defines
from sqlalchemy import event
from sqlalchemy.engine import Engine
from urllib.parse import urlparse

from pinpointPy.Common import Trace, PinTrace
from pinpointPy.Fastapi.AsyCommon import AsyncPinTrace
from pinpointPy.TraceContext import _reqTraceCtx


class CreateEnginePlugin(Trace):
    def onBefore(self, *args, **kwargs):
        return args, kwargs

    def onEnd(self, ret):
        if isinstance(ret, Engine):
            event.listen(ret, "before_cursor_execute", before_cursor_execute)
            event.listen(ret, "after_cursor_execute", after_cursor_execute)
        return ret


class _sqlalchemyPlugin(PinTrace):
    def __init__(self, name):
        super().__init__(name)
        self.trace_id = -1

    def onBefore(self, parentId: int, *args, **kwargs):
        trace_id, args, kwargs = super().onBefore(parentId, *args, **kwargs)
        statement = args[2]
        conn = args[0]
        context = args[4]
        pinpoint.add_trace_header(
            Defines.PP_INTERCEPTOR_NAME, self.getUniqueName(), trace_id=trace_id)
        pinpoint.add_trace_header(
            Defines.PP_SQL_FORMAT, statement, trace_id=trace_id)
        # pinpoint.add_trace_header_v2(
        # Defines.PP_ARGS, 'user not cared', trace_id=trace_id)
        DBUrl = urlparse(str(conn.engine.url))
        if 'mysql' in DBUrl.scheme:
            pinpoint.add_trace_header(
                Defines.PP_SERVER_TYPE, Defines.PP_MYSQL, trace_id=trace_id)

        if 'postgresql' in DBUrl.scheme:
            pinpoint.add_trace_header(
                Defines.PP_SERVER_TYPE, Defines.PP_POSTGRESQL, trace_id=trace_id)

        pinpoint.add_trace_header(
            Defines.PP_DESTINATION, DBUrl.hostname, trace_id=trace_id)
        self.trace_id = trace_id
        return trace_id, args, kwargs

    def onEnd(self, traceId, ret):
        super().onEnd(self.trace_id, ret)


@event.listens_for(Engine, "before_cursor_execute")
def before_cursor_execute(conn, cursor, statement,
                          parameters, context, executemany):
    if hasattr(cursor, '_pinpoint_') and cursor._pinpoint_ != None:
        return
    plugins = _sqlalchemyPlugin('cursor_execute')
    sample, parent_id, _, _ = plugins.isSample()
    if sample:
        plugins.onBefore(parent_id, conn, cursor, statement,
                         parameters, context, executemany)
        # skipped all return value, as not need

    cursor._pinpoint_ = plugins


@event.listens_for(Engine, identifier="after_cursor_execute")
def after_cursor_execute(conn, cursor, statement,
                         parameters, context, executemany):
    if hasattr(cursor, '_pinpoint_') and isinstance(cursor._pinpoint_, PinTrace):
        cursor._pinpoint_.onEnd(-1, None)
        cursor._pinpoint_ = None
