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


from ... import pinpoint
from ... import Defines
from sqlalchemy import event
from sqlalchemy.engine import Engine
from urllib.parse import urlparse



@event.listens_for(Engine, "before_cursor_execute")
def before_cursor_execute(conn, cursor, statement,
                        parameters, context, executemany):
    pinpoint.with_trace()
    pinpoint.add_trace_header(Defines.PP_INTERCEPTOR_NAME, 'before_cursor_execute')
    pinpoint.add_trace_header(Defines.PP_SQL_FORMAT, statement)
    pinpoint.add_trace_header_v2(Defines.PP_ARGS, 'user not cared')
    DBUrl = urlparse(str(conn.engine.url))
    if 'mysql'  in DBUrl.scheme:
        pinpoint.add_trace_header(Defines.PP_SERVER_TYPE, Defines.PP_MYSQL)
    
    if 'postgresql' in DBUrl.scheme:
        pinpoint.add_trace_header(Defines.PP_SERVER_TYPE, Defines.PP_POSTGRESQL)

    pinpoint.add_trace_header(Defines.PP_DESTINATION, DBUrl.hostname)

@event.listens_for(Engine, "after_cursor_execute")
def after_cursor_execute(conn, cursor, statement,
                        parameters, context, executemany):
    pinpoint.end_trace()

