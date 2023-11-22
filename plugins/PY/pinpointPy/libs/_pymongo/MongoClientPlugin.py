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

from pinpointPy import Common, pinpoint, Defines, get_logger
from pymongo import monitoring


class MongoClientPlugin(Common.PinTrace):
    def __init__(self, name):
        super().__init__(name)
        self.trace_id = -1

    def onBefore(self, parentId, *args, **kwargs):
        traceId, args, kwargs = super().onBefore(parentId, *args, **kwargs)
        event = args[0]
        assert isinstance(event, monitoring.CommandStartedEvent)
        pinpoint.add_trace_header(
            Defines.PP_INTERCEPTOR_NAME, event.command_name, traceId)
        pinpoint.add_trace_header(
            Defines.PP_SERVER_TYPE, Defines.PP_MONGDB_EXE_QUERY, traceId)
        pinpoint.add_trace_header(
            Defines.PP_DESTINATION, event.database_name, traceId)
        self.trace_id = traceId
        return traceId, args, kwargs

    def onEnd(self, traceId, ret):
        super().onEnd(self.trace_id, ret)
        return ret

    def onException(self, traceId, e):
        pinpoint.add_trace_header(
            Defines.PP_ADD_EXCEPTION, str(e), self.trace_id)


class CommandLogger(monitoring.CommandListener):

    def __init__(self) -> None:
        self.request_id_map = {}

    def started(self, event):
        sampled, parentId, _, _ = MongoClientPlugin.isSample()
        if not sampled:
            return
        request_id = event.request_id

        if request_id in self.request_id_map:
            get_logger().info("started listener called with wrong order, maybe a bug")
            return

        watcher = MongoClientPlugin("monitoring.CommandListener")
        watcher.onBefore(parentId, event)
        self.request_id_map[request_id] = watcher

    def succeeded(self, event):
        request_id = event.request_id
        if request_id in self.request_id_map:
            watcher = self.request_id_map[request_id]
            watcher.onEnd(0, None)
            del self.request_id_map[request_id]

    def failed(self, event):
        request_id = event.request_id
        if request_id in self.request_id_map:
            watcher = self.request_id_map[request_id]
            watcher.onException(0, event.failure)
            watcher.onEnd(0, None)
            del self.request_id_map[request_id]
