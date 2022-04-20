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
from ... import Defines
from ... import pinpoint


class ViewsPlugin(Common.PinTrace):

    def __init__(self,name):
        super().__init__(name)

    def onBefore(self,*args, **kwargs):
        super().onBefore(*args, **kwargs)
        ###############################################################
        pinpoint.add_trace_header(Defines.PP_INTERCEPTOR_NAME, args[0].get_view_name())
        pinpoint.add_trace_header(Defines.PP_SERVER_TYPE, Defines.PP_METHOD_CALL)
        ###############################################################
        url = '{%s %r}' % (args[1].method,args[1].get_full_path())
        pinpoint.add_trace_header_v2(Defines.PP_ARGS, url)
        return args,kwargs

    def onEnd(self,ret):
        super().onEnd(ret)
        return ret

    def onException(self, e):
        pinpoint.add_trace_header(Defines.PP_ADD_EXCEPTION, str(e))
