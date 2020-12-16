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


from .FlaskPlugins import BaseFlaskPlugins

from pinpoint.common import *
import pinpointPy

class PinPointMiddleWare():

    def __init__(self, app,wsgi_app):
        self.app = app
        self.wsgi_app = wsgi_app

        @app.after_request
        def mark_status_code(response):
            if response:
                pinpointPy.add_clues(PP_HTTP_STATUS_CODE, str(response.status))
            return response

    @BaseFlaskPlugins("Flask Web App")
    def __call__(self, environ, start_response):
        return self.wsgi_app(environ, start_response)

