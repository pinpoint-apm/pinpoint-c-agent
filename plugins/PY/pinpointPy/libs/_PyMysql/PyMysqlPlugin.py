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


from pinpointPy import Common
from pinpointPy import pinpoint
from pinpointPy import Defines


class PyMysqlPlugin(Common.PinTrace):

    def __init__(self, name):
        super().__init__(name)

    def onBefore(self, parentId, *args, **kwargs):
        trace_id, _, _ = super().onBefore(parentId, *args, **kwargs)
        ###############################################################
        pinpoint.add_trace_header(
            Defines.PP_INTERCEPTOR_NAME, self.getUniqueName(), trace_id)
        pinpoint.add_trace_header(
            Defines.PP_SERVER_TYPE, Defines.PP_MYSQL, trace_id)
        pinpoint.add_trace_header(Defines.PP_SQL_FORMAT, args[1], trace_id)
        ###############################################################
        cursor = args[0]
        dst = cursor.connection.get_host_info()
        pinpoint.add_trace_header(
            Defines.PP_DESTINATION, dst, trace_id=trace_id)
        return trace_id, args, kwargs

    def onEnd(self, trace_id, ret):
        super().onEnd(trace_id, ret)
        return ret

    def onException(self, trace_id, e):
        pinpoint.add_trace_header(Defines.PP_ADD_EXCEPTION, str(e), trace_id)
