#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 12/16/20

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


from ... import Common
from ... import pinpoint
from ... import Defines


class MysqldbPlugin(Common.PinTrace):
    def __init__(self, name):
        super().__init__(name)
        self.dst = ''

    def onBefore(self, *args, **kwargs):
        super().onBefore(*args, **kwargs)
        connection = args[0]
        self.dst = connection.get_host_info()
        ###############################################################
        pinpoint.add_trace_header(Defines.PP_INTERCEPTOR_NAME, self.getFuncUniqueName())
        pinpoint.add_trace_header(Defines.PP_SERVER_TYPE, Defines.PP_MYSQL)
        query = args[1]
        ## copy from MySQLdb  def query(self, query):
        if isinstance(query, bytearray):
            query = bytes(query)
        pinpoint.add_trace_header(Defines.PP_SQL_FORMAT, query.decode('utf-8'))
        ###############################################################
        pinpoint.add_trace_header(Defines.PP_DESTINATION, self.dst)
        return args, kwargs

    def onEnd(self, ret):
        super().onEnd(ret)
        return ret

    def onException(self, e):
        pinpoint.add_trace_header(Defines.PP_ADD_EXCEPTION, str(e))
