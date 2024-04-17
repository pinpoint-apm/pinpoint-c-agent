#!/usr/bin/env python
# -*- coding: UTF-8 -*-

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

from pinpointPy import Common, pinpoint, Defines
from functools import wraps
import psycopg2


class QueryPlugin(Common.PinTrace):

    def __init__(self, name):
        super().__init__(name)

    def onBefore(self, parentId, *args, **kwargs):
        trace_id, _, _ = super().onBefore(parentId, *args, **kwargs)
        ###############################################################
        pinpoint.add_trace_header(
            Defines.PP_INTERCEPTOR_NAME, self.getUniqueName(), trace_id)
        pinpoint.add_trace_header(
            Defines.PP_SERVER_TYPE, Defines.PP_POSTGRESQL, trace_id)
        query = str(args[1])
        pinpoint.add_trace_header(Defines.PP_SQL_FORMAT, query, trace_id)
        ###############################################################
        cursor = args[0]
        dst = cursor.connection.get_dsn_parameters()['host']
        pinpoint.add_trace_header(
            Defines.PP_DESTINATION, dst, trace_id=trace_id)
        return trace_id, args, kwargs

    def onEnd(self, trace_id, ret):
        super().onEnd(trace_id, ret)
        return ret

    def onException(self, trace_id, e):
        pinpoint.add_trace_header(Defines.PP_ADD_EXCEPTION, str(e), trace_id)


class FetchPlugin(QueryPlugin):
    def onBefore(self, parentId, *args, **kwargs: None):
        trace_id, _, _ = super(QueryPlugin, self).onBefore(
            parentId, *args, **kwargs)
        pinpoint.add_trace_header(
            Defines.PP_INTERCEPTOR_NAME, self.getUniqueName(), trace_id)
        return trace_id, args, kwargs


class LoggingCursor(psycopg2.extensions.cursor):
    @QueryPlugin("execute")
    def execute(self, sql, args=None):
        return super().execute(sql, args)

    @QueryPlugin("executemany")
    def executemany(self, query,  args=None):
        return super().executemany(query, args)

    @QueryPlugin("mogrify")
    def mogrify(self, query, args=None):
        return super().mogrify(query, args)

    @FetchPlugin("fetchall")
    def fetchall(self):
        return super().fetchall()

    @FetchPlugin("fetchmany")
    def fetchmany(self, size: None):
        return super().fetchmany(size)

    @FetchPlugin("fetchone")
    def fetchone(self):
        return super().fetchone()


class ConnectionPlugin:
    def __init__(self, name):
        pass

    def __call__(self, func):

        @wraps(func)
        def pinpointTrace(*args, **kwargs):
            kwargs['cursor_factory'] = LoggingCursor
            ret = func(*args, **kwargs)
            return ret
        return pinpointTrace
