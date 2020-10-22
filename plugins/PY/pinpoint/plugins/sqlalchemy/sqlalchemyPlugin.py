#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 7/31/20

# ------------------------------------------------------------------------------
#  Copyright  2020. NAVER Corp.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
# ------------------------------------------------------------------------------

from ..PinpointDefine import *
from sqlalchemy import event
from sqlalchemy.engine import Engine
from urllib.parse import urlparse
import pinpointPy

@event.listens_for(Engine, "before_cursor_execute")
def before_cursor_execute(conn, cursor, statement,
                        parameters, context, executemany):
    pinpointPy.start_trace()
    pinpointPy.add_clue(FuncName, 'before_cursor_execute')
    pinpointPy.add_clue(SQL_STR,statement)
    pinpointPy.add_clues(PY_ARGS, 'user not cared')
    DBUrl = urlparse(str(conn.engine.url))
    if 'mysql'  in DBUrl.scheme:
        pinpointPy.add_clue(ServerType, MYSQL)
    #todo add more mssql/oracle

    pinpointPy.add_clue(DESTINATION,DBUrl.hostname)
    # print(conn)
    # print(cursor)
    # print(statement)
    # print(parameters)
    # print(context)
    # print(executemany)

@event.listens_for(Engine, "after_cursor_execute")
def after_cursor_execute(conn, cursor, statement,
                        parameters, context, executemany):
    pinpointPy.end_trace()
    # print(conn.engine.url)
    # print(cursor)
    # print(statement)
    # print(parameters)
    # print(context)
    # print(executemany)