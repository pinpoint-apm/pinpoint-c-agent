#!/usr/bin/env python
# -*- coding: UTF-8 -*-

# Created by eeliu at 7/31/20


# ******************************************************************************
#   Copyright  2020. NAVER Corp.
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
# ******************************************************************************

from pinpoint.common import *
from sqlalchemy import event
from sqlalchemy.engine import Engine
from urllib.parse import urlparse
import pinpointPy


@event.listens_for(Engine, "before_cursor_execute")
def before_cursor_execute(conn, cursor, statement,
                        parameters, context, executemany):
    pinpointPy.start_trace()
    pinpointPy.add_clue(PP_INTERCEPTOR_NAME, 'before_cursor_execute')
    pinpointPy.add_clue(PP_SQL_FORMAT,statement)
    pinpointPy.add_clues(PP_ARGS, 'user not cared')
    DBUrl = urlparse(str(conn.engine.url))
    if 'mysql'  in DBUrl.scheme:
        pinpointPy.add_clue(PP_SERVER_TYPE, PP_MYSQL)
    
    if 'postgresql' in DBUrl.scheme:
        pinpointPy.add_clue(PP_SERVER_TYPE, PP_POSTGRESQL)

    pinpointPy.add_clue(PP_DESTINATION,DBUrl.hostname)

@event.listens_for(Engine, "after_cursor_execute")
def after_cursor_execute(conn, cursor, statement,
                        parameters, context, executemany):
    pinpointPy.end_trace()

