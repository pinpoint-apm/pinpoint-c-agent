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
from ... import Helper
from ... import Defines
from ... import pinpoint

from urllib.parse import urlparse


class NextSpanPlugin(Common.PinTrace):

    def __init__(self, name):
        super().__init__(name)

    def isSample(self, args):
        '''
        if not root, no trace
        :return:
        '''
        url = args[0][0]
        target = urlparse(url).netloc
        kwargs = args[1]
        if "headers" not in kwargs:
            kwargs["headers"] = {}
        if pinpoint.get_context(Defines.PP_HEADER_PINPOINT_SAMPLED) == "s1":
            Helper.generatePinpointHeader(target, kwargs['headers'])
            return True
        else:
            kwargs['headers'][Defines.PP_HEADER_PINPOINT_SAMPLED] = Defines.PP_NOT_SAMPLED
            return False

    def onBefore(self, *args, **kwargs):
        url = args[0]
        target = urlparse(url).netloc
        super().onBefore(*args, **kwargs)
        ###############################################################
        pinpoint.add_trace_header(Defines.PP_INTERCEPTOR_NAME, self.getFuncUniqueName())
        pinpoint.add_trace_header(Defines.PP_SERVER_TYPE, Defines.PP_REMOTE_METHOD)
        pinpoint.add_trace_header_v2(Defines.PP_ARGS, url)
        pinpoint.add_trace_header_v2(Defines.PP_HTTP_URL, url)
        pinpoint.add_trace_header(Defines.PP_DESTINATION, target)
        ###############################################################
        return args, kwargs

    def onEnd(self, ret):
        ###############################################################
        pinpoint.add_trace_header(Defines.PP_NEXT_SPAN_ID, pinpoint.get_context(Defines.PP_NEXT_SPAN_ID))
        pinpoint.add_trace_header_v2(Defines.PP_HTTP_STATUS_CODE, str(ret.status_code))
        pinpoint.add_trace_header_v2(Defines.PP_RETURN, str(ret))
        ###############################################################
        super().onEnd(ret)
        return ret

    def onException(self, e):
        pinpoint.add_trace_header(Defines.PP_ADD_EXCEPTION, str(e))

    def get_arg(self, *args, **kwargs):
        args_tmp = {}
        j = 0

        for i in args:
            args_tmp["arg[" + str(j) + "]"] = (str(i))
            j += 1

        for k in kwargs:
            args_tmp[k] = kwargs[k]

        return str(args_tmp)
