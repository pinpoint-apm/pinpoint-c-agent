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


from pinpointPy import Common, Helper, Defines, pinpoint
from urllib.parse import urlparse


class NextSpanPlugin(Common.PinTrace):

    def __init__(self, name):
        super().__init__(name)

    @staticmethod
    def isSample(*args, **kwargs):
        sampled, parentId, _, _ = Common.PinTrace.isSample(args, kwargs)
        headers = {}
        if 'headers' in kwargs:
            headers = kwargs['headers']
        if Defines.PP_HEADER_PINPOINT_SAMPLED in headers:
            return False, parentId, args, kwargs
        # pull out headers
        if sampled:
            url = args[1]
            target = urlparse(url).netloc
            if pinpoint.get_context(Defines.PP_HEADER_PINPOINT_SAMPLED, parentId) == "s1":
                Helper.generatePinpointHeader(
                    target, headers, traceId=parentId)
        else:
            headers[Defines.PP_HEADER_PINPOINT_SAMPLED] = Defines.PP_NOT_SAMPLED
        kwargs['headers'] = headers
        return sampled, parentId, args, kwargs

    def onBefore(self, parentId, *args, **kwargs):
        traceId, _args, _kwargs = super().onBefore(parentId, *args, **kwargs)
        url = _args[1]
        target = urlparse(url).netloc
        ###############################################################
        pinpoint.add_trace_header(
            Defines.PP_INTERCEPTOR_NAME, self.getUniqueName(), traceId)
        pinpoint.add_trace_header(
            Defines.PP_SERVER_TYPE, Defines.PP_REMOTE_METHOD, traceId)
        pinpoint.add_trace_header_v2(Defines.PP_ARGS, url, traceId)
        pinpoint.add_trace_header_v2(Defines.PP_HTTP_URL, url, traceId)
        pinpoint.add_trace_header(Defines.PP_DESTINATION, target, traceId)
        ###############################################################
        return traceId, _args, _kwargs

    def onEnd(self, traceId, ret):
        ###############################################################
        pinpoint.add_trace_header(Defines.PP_NEXT_SPAN_ID, pinpoint.get_context(
            Defines.PP_NEXT_SPAN_ID, traceId), traceId)
        if ret:
            pinpoint.add_trace_header_v2(
                Defines.PP_HTTP_STATUS_CODE, str(ret.status_code), traceId)
            pinpoint.add_trace_header_v2(Defines.PP_RETURN, str(ret), traceId)
        ###############################################################
        super().onEnd(traceId, ret)
        return ret

    def onException(self, traceId, e):
        pinpoint.add_trace_header(Defines.PP_ADD_EXCEPTION, str(e), traceId)
