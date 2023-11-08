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

from pinpointPy.Common import Trace
from pinpointPy.Fastapi.AsyCommon import AsyncPinTrace, TraceContext


class CreateEnginePlugin(Trace):
    def onBefore(self, *args, **kwargs):
        pass

    def onEnd(self, ret):
        if isinstance(ret, Engine):
            event.listen(ret, "before_cursor_execute", before_cursor_execute)
            event.listen(ret, "after_cursor_execute", after_cursor_execute)
        return ret


@event.listens_for(Engine, "before_cursor_execute")
def before_cursor_execute(conn, cursor, statement,
                          parameters, context, executemany):
    hasParent, parentId, _, _ = AsyncPinTrace.isSample()
    if hasParent:
        trace_id = pinpoint.with_trace(parentId)
        TraceContext.set_parent_id(trace_id)
        pinpoint.add_trace_header(
            Defines.PP_INTERCEPTOR_NAME, 'before_cursor_execute', trace_id=trace_id)
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
        context.__pinpoint__ = trace_id


@event.listens_for(Engine, "after_cursor_execute")
def after_cursor_execute(conn, cursor, statement,
                         parameters, context, executemany):
    if hasattr(context, '__pinpoint__'):
        trace_id = pinpoint.end_trace(context.__pinpoint__)
        TraceContext.set_parent_id(trace_id)
