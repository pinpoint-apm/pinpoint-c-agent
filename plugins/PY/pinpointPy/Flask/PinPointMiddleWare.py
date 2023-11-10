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


#!/usr/bin/env python
# -*- coding: UTF-8 -*-
from pinpointPy.Defines import PP_HTTP_STATUS_CODE, PP_URL_TEMPLATED
from pinpointPy.pinpoint import add_trace_header_v2, add_trace_header
from pinpointPy.Flask.FlaskPlugins import BaseFlaskPlugins
from flask import request
import sys
from pinpointPy.TraceContext import get_trace_context


class PinPointMiddleWare():

    def __init__(self, app, wsgi_app):
        self.app = app
        self.wsgi_app = wsgi_app

        @app.after_request
        def mark_status_code(response):
            status, trace_id = get_trace_context().get_parent_id()
            if status:
                if request.url_rule:
                    add_trace_header(PP_URL_TEMPLATED, str(
                        request.url_rule), trace_id)
                    if 'unittest' in sys.modules.keys():
                        response.headers["UT"] = str(request.url_rule)
                if response:
                    add_trace_header_v2(PP_HTTP_STATUS_CODE,
                                        str(response.status), trace_id)
            return response

    @BaseFlaskPlugins("Flask Web App")
    def __call__(self, environ, start_response):
        return self.wsgi_app(environ, start_response)
