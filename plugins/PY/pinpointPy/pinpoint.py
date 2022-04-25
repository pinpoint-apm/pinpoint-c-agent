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
# !/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 4/19/22

import random
import _pinpointPy
from collections.abc import Callable

__app_id = 'app_id_str'
__app_name = 'app_name_str'


def app_id():
    global __app_id
    return __app_id


def app_name():
    global __app_name
    return __app_name


def with_trace(trace_id=-1):
    return _pinpointPy.start_trace(trace_id)


def end_trace(trace_id=-1):
    return _pinpointPy.end_trace(trace_id)


def add_trace_header(key: str, value: str, trace_id: int = -1, location: int = 0):
    _pinpointPy.add_clue(key, value, trace_id, location)


def add_trace_header_v2(key: str, value: str, trace_id: int = -1, location: int = 0):
    _pinpointPy.add_clues(key, value, trace_id, location)


def add_context(key: str, value: str, trace_id: int = -1):
    _pinpointPy.set_context_key(key, value, trace_id)


def get_context(key: str, trace_id: int = -1):
    return _pinpointPy.get_context_key(key,trace_id)


def gen_tid():
    global __app_id
    return '%s^%s^%s' % (__app_id, str(_pinpointPy.start_time()), str(_pinpointPy.unique_id()))


def gen_sid():
    return str(random.randint(0, 2147483647))


def trace_has_root(trace_id=-1):
    return _pinpointPy.trace_has_root(trace_id)


def mark_as_error(message: str, filename: str = '', line: int = 0):
    _pinpointPy.mark_as_error(message, filename, line)


def drop_trace(trace_id: int = -1):
    _pinpointPy.drop_trace(trace_id)


def check_trace_limit(time: int=-1) -> bool:
    return _pinpointPy.check_tracelimit(time)


def set_agent(app_id_str: str, app_name_str: str, collect_agent_host: str, trace_limit: int = -1, debug_callback=True):
    global __app_id, __app_name
    __app_id = app_id_str
    __app_name = app_name_str
    _pinpointPy.set_agent(collect_agent_host, trace_limit)
    if type(debug_callback) == bool and debug_callback:
        _pinpointPy.enable_debug(debug_callback)
    elif callable(debug_callback):
        # todo : check type is [[str]->]
        _pinpointPy.enable_debug(debug_callback)
