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

from pinpointPy.Common import PinTrace
from pinpointPy import Helper, pinpoint


class WSGIPlugin(PinTrace):
    def __init__(self, name):
        super().__init__(name)

    @staticmethod
    def isSample(*args, **kwargs):
        return True, 0, args, kwargs

    def onBefore(self, parentId, *args, **kwargs):
        trace_id, _, _ = super().onBefore(parentId, *args, **kwargs)
        environ = args[0]
        Helper.startPinpointByEnviron(environ, trace_id)

        return trace_id, args, kwargs

    def onEnd(self, trace_id, ret):
        Helper.endPinpointByEnviron(ret, trace_id)
        super().onEnd(ret, trace_id)
        return ret

    def onException(self, traceId, e):
        pinpoint.mark_as_error(str(e), "", traceId)
