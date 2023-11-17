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

# Created by eeliu at 11/9/2023

from pinpointPy.pinpoint import get_logger
import threading

_local_id = threading.local()


class TraceContext:
    def get_parent_id(self):
        raise NotImplementedError("get_parent_id")

    def set_parent_id(self, id: int) -> int:
        raise NotImplementedError("set_parent_id")


_reqTraceCtx = TraceContext()


def get_trace_context() -> TraceContext:
    global _reqTraceCtx
    return _reqTraceCtx


def set_trace_context(new_trace_context: TraceContext):
    global _reqTraceCtx
    if isinstance(new_trace_context, TraceContext):
        _reqTraceCtx = new_trace_context
    else:
        raise ValueError(
            f'Must be pinpointPy.TraceContext type: {new_trace_context}')
    get_logger().debug("trace context id changed")


class thread_local_context (TraceContext):
    def get_parent_id(self):
        global _local_id
        if getattr(_local_id, '_pinpoint_id_', None):
            return True, _local_id._pinpoint_id_
        else:
            return False, -1

    def set_parent_id(self, id: int):
        global _local_id
        _local_id._pinpoint_id_ = id
