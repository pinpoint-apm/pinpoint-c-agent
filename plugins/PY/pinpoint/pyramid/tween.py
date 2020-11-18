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

# Created by eeliu at 11/12/20

from .PyRequestPlugin import PyRequestPlugin
from pinpoint.libs import monkey_patch_for_pinpoint
monkey_patch_for_pinpoint()

def pinpoint_tween(handler,registry):

    def tween(request):
        plugin = PyRequestPlugin("pinpoint_tween")
        plugin.onBefore(request)

        response = handler(request)
        plugin.onEnd(response)

        return response

    return tween
