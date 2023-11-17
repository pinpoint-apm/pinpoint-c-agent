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
from pinpointPy import Common, pinpoint, Defines


class MySQLdbPlugin(Common.PinTrace):
    def __init__(self, name):
        super().__init__(name)

    def onBefore(self, parentId, *args, **kwargs):
        trace_id, args, kwargs = super().onBefore(parentId, *args, **kwargs)
        connection = args[0]
        self.dst = connection.get_host_info()

        pinpoint.add_trace_header(
            Defines.PP_INTERCEPTOR_NAME, self.getUniqueName(), trace_id)
        pinpoint.add_trace_header(
            Defines.PP_SERVER_TYPE, Defines.PP_MYSQL, trace_id)
        query = args[1]
        # copy from MySQLdb  def query(self, query):
        if isinstance(query, bytearray):
            query = bytes(query)
        pinpoint.add_trace_header(
            Defines.PP_SQL_FORMAT, query.decode('utf-8'), trace_id)

        pinpoint.add_trace_header(Defines.PP_DESTINATION, self.dst, trace_id)
        return trace_id, args, kwargs

    def onEnd(self, trace_id, ret):
        super().onEnd(trace_id, ret)
        return ret

    def onException(self, trace_id, e):
        pinpoint.add_trace_header(Defines.PP_ADD_EXCEPTION, str(e), trace_id)
