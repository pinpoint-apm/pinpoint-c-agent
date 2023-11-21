#!/usr/bin/env python
# -*- coding: UTF-8 -*-

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

# Created by eeliu at 8/20/20
from urllib.parse import urlparse
from pinpointPy import Common, Defines, pinpoint, Helper


class RequestPlugin(Common.PinTrace):

    @staticmethod
    def get_url(*args, **kwargs):
        url = "/"
        if 'url' in kwargs:
            url = kwargs['url']
        else:
            if len(args) >= 2:
                url = args[1]
        return url

    def __init__(self, name):
        super().__init__(name)

    # reference from https://docs.python.org/3/library/urllib.request.html#urllib.request.urlopen
    # as url support both url/Request, UrlOpenPlugin will change url to request for adding header
    @staticmethod
    def isSample(*args, **kwargs):
        sampled, parentId, _, _ = Common.PinTrace.isSample(args, kwargs)
        headers = {}
        if 'headers' in kwargs:
            headers = kwargs['headers']
        elif len(args) >= 4:
            headers = args[3]
        if Defines.PP_HEADER_PINPOINT_SAMPLED in headers:
            return False, parentId, args, kwargs
        # pull out headers
        if sampled:
            url = RequestPlugin.get_url(*args, **kwargs)
            target = urlparse(url).netloc
            if pinpoint.get_context(Defines.PP_HEADER_PINPOINT_SAMPLED, parentId) == "s1":
                Helper.generatePinpointHeader(
                    target, headers=headers, traceId=parentId)
        else:
            headers[Defines.PP_HEADER_PINPOINT_SAMPLED] = Defines.PP_NOT_SAMPLED
        # push back headers
        if len(args) >= 4:
            _args = list(args)
            _args[3] = headers
            args = tuple(_args)
        else:
            kwargs['headers'] = headers

        return sampled, parentId, args, kwargs

    def onBefore(self, parentId, *args, **kwargs):
        traceId, args, kwargs = super().onBefore(parentId, *args, **kwargs)
        url = RequestPlugin.get_url(*args, **kwargs)
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
        return traceId, args, kwargs

    def onEnd(self, traceId, ret):
        ###############################################################

        pinpoint.add_trace_header(
            Defines.PP_NEXT_SPAN_ID, pinpoint.get_context(Defines.PP_NEXT_SPAN_ID, traceId), traceId)
        # pinpoint.add_trace_header_v2(
        #     Defines.PP_HTTP_STATUS_CODE, str(ret.status_code), traceId)
        pinpoint.add_trace_header_v2(Defines.PP_RETURN, str(ret), traceId)
        ###############################################################
        super().onEnd(traceId, ret)
        return ret

    def onException(self, traceId, e):
        pinpoint.add_trace_header(Defines.PP_ADD_EXCEPTION, str(e), traceId)
