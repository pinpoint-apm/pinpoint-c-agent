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

from MySQLdb.cursors import BaseCursor
from pinpointPy import Common, pinpoint, Defines

# reference from https://github.com/PyMySQL/mysqlclient/blob/3805b28abc24cc985ffe7c8a05dd4bd6530aefe8/src/MySQLdb/cursors.py#L162


class BaseCursorPlugins(Common.PinTrace):
    def __init__(self, name):
        super().__init__(name)

    def onBefore(self, parentId, *args, **kwargs):
        trace_id, args, kwargs = super().onBefore(parentId, *args, **kwargs)

        assert isinstance(args[0], BaseCursor)

        dst = args[0].connection.get_host_info()

        pinpoint.add_trace_header(
            Defines.PP_INTERCEPTOR_NAME, self.getUniqueName(), trace_id)
        pinpoint.add_trace_header(
            Defines.PP_SERVER_TYPE, Defines.PP_MYSQL, trace_id)
        query = args[1]
        pinpoint.add_trace_header(
            Defines.PP_SQL_FORMAT, query, trace_id)

        pinpoint.add_trace_header(Defines.PP_DESTINATION, dst, trace_id)
        return trace_id, args, kwargs

    def onEnd(self, trace_id, ret):
        super().onEnd(trace_id, ret)
        return ret

    def onException(self, trace_id, e):
        pinpoint.add_trace_header(Defines.PP_ADD_EXCEPTION, str(e), trace_id)
