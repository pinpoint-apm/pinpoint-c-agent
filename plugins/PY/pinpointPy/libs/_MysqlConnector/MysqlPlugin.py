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


class MysqlPlugin(Common.PinTrace):

    def __init__(self, name):
        super().__init__(name)

    def onBefore(self, parentId, *args, **kwargs):
        traceId, args, kwargs = super().onBefore(parentId, *args, **kwargs)
        ###############################################################
        pinpoint.add_trace_header(
            Defines.PP_INTERCEPTOR_NAME, self.getUniqueName(), traceId)
        pinpoint.add_trace_header(
            Defines.PP_SERVER_TYPE, Defines.PP_MYSQL, traceId)
        pinpoint.add_trace_header(Defines.PP_SQL_FORMAT, args[1], traceId)
        import sys
        if 'unittest' in sys.modules.keys():
            from mysql.connector.cursor import MySQLCursor
            if isinstance(args[0], MySQLCursor):
                cursor = args[0]
                cursor._pinpoint_ = True

        ###############################################################
        dst = self.get_mysql_server_host(args[0])
        pinpoint.add_trace_header(Defines.PP_DESTINATION, dst, traceId)
        return traceId, args, kwargs

    def onEnd(self, traceId, ret):
        super().onEnd(traceId, ret)
        return ret

    def onException(self, traceId, e):
        pinpoint.add_trace_header(Defines.PP_ADD_EXCEPTION, f'{e}', traceId)

    def get_mysql_server_host(self, cursor):
        return f'{cursor._connection.server_host}:{cursor._connection.server_port}'
