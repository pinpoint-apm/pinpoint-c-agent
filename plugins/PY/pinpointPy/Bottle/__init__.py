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

# Created by eeliu at 11/9/20

from pinpointPy.Bottle.RequestPlugin import RequestPlugin


def PinPointMiddleWare(application):
    @RequestPlugin("pyweb-request")
    def handler(environ, start_response):
        return application(environ, start_response)
    return handler


__all__ = ['PinPointMiddleWare']
__version__ = '0.0.2'
__author__ = 'liu.mingyi@navercorp.com'
