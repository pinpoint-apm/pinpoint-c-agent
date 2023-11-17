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

from pinpointPy import Common, Defines, pinpoint


class CMysqlPlugin(Common.PinTrace):

    def __init__(self, name):
        super().__init__(name)

    # -> tuple[Any, tuple[Any, ...], dict[str, Any]]:
    def onBefore(self, parentId, *args, **kwargs):
        trace_id, args, kwargs = super().onBefore(parentId, *args, **kwargs)
        pinpoint.add_trace_header(
            Defines.PP_INTERCEPTOR_NAME, self.getUniqueName(), trace_id)
        pinpoint.add_trace_header(
            Defines.PP_SERVER_TYPE, Defines.PP_MYSQL, trace_id)
        pinpoint.add_trace_header(Defines.PP_SQL_FORMAT, args[1], trace_id)
        import sys
        if 'unittest' in sys.modules.keys():
            from mysql.connector.cursor_cext import CMySQLCursor
            if isinstance(args[0], CMySQLCursor):
                cursor = args[0]
                cursor._pinpoint_ = True
        dst = self.get_cursor_host(args[0])
        pinpoint.add_trace_header(Defines.PP_DESTINATION, dst, trace_id)
        return trace_id, args, kwargs

    def onEnd(self, traceId, ret):
        super().onEnd(traceId, ret)
        return ret

    def onException(self, traceId, e):
        pinpoint.add_trace_header(Defines.PP_ADD_EXCEPTION, f'{e}', traceId)

    def get_cursor_host(self, cursor):
        return f'{cursor._cnx.server_host}:{cursor._cnx.server_port}'
