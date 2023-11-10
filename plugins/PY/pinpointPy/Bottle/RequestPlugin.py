#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 11/9/20
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

from bottle import response

from pinpointPy.Defines import PP_HTTP_STATUS_CODE, PP_ADD_EXCEPTION
from pinpointPy.WSGIPlugin import WSGIPlugin
from pinpointPy.pinpoint import add_trace_header_v2, add_trace_header


class RequestPlugin(WSGIPlugin):

    def onEnd(self, trace_id, ret):
        add_trace_header_v2(PP_HTTP_STATUS_CODE, str(
            response.status_code), trace_id)
        if response.status_code != 200:
            add_trace_header(PP_ADD_EXCEPTION,
                             "status code isn't 200", trace_id)
        return super().onEnd(trace_id, ret)
