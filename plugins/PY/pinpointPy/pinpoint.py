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

from tempfile import mkstemp
import random
import logging
import _pinpointPy
__app_id = 'app_id_str'
__app_name = 'app_name_str'


def init_logger():
    logger = logging.getLogger('pinpoint')
    logger.setLevel(logging.DEBUG)
    formatter = logging.Formatter(
        '%(asctime)s %(levelname)s %(message)s')
    _, filepath = mkstemp(prefix="pinpoint")
    import sys
    if 'unittest' in sys.modules.keys():
        file_handler = logging.FileHandler(filepath)
        print(filepath)
        file_handler.setFormatter(formatter)
        file_handler.setLevel(level=logging.DEBUG)
        logger.addHandler(file_handler)
    else:
        ch = logging.StreamHandler()
        ch.setLevel(logging.DEBUG)
        ch.setFormatter(formatter)
        logger.addHandler(ch)


init_logger()


def get_logger() -> logging.Logger:
    return logging.getLogger('pinpoint')


def set_logger_level(level=logging.INFO):
    get_logger().setLevel(level)


def app_id():
    global __app_id
    return __app_id


def app_name():
    global __app_name
    return __app_name


def with_trace(trace_id: int) -> int:
    return _pinpointPy.start_trace(trace_id)


def end_trace(trace_id: int) -> int:  # -> Any:
    return _pinpointPy.end_trace(trace_id)


def add_trace_header(key: str, value: str, trace_id: int, location: int = 0):
    _pinpointPy.add_clue(key, value, trace_id, location)


def add_trace_header_v2(key: str, value: str, trace_id: int, location: int = 0):
    _pinpointPy.add_clues(key, value, trace_id, location)


def add_context(key: str, value: str, trace_id: int):
    _pinpointPy.set_context_key(key, value, trace_id)


def get_context(key: str, trace_id: int):
    return _pinpointPy.get_context_key(key, trace_id)


def gen_tid() -> str:
    global __app_id
    return '%s^%s^%s' % (__app_id, str(_pinpointPy.start_time()), str(_pinpointPy.unique_id()))


def gen_sid() -> str:
    return str(random.randint(0, 2147483647))


def trace_has_root(trace_id=-1):
    return _pinpointPy.trace_has_root(trace_id)


def mark_as_error(message: str, filename: str = '', trace_id: int = -1, line: int = 0):
    _pinpointPy.mark_as_error(message, filename, line, trace_id)


def drop_trace(trace_id: int):
    _pinpointPy.drop_trace(trace_id)


def check_trace_limit(time: int = -1) -> bool:
    return _pinpointPy.check_tracelimit(time)


def set_agent(app_id_str: str, app_name_str: str, collect_agent_host: str,  trace_limit: int = -1, log_level=logging.INFO):
    global __app_id, __app_name
    __app_id = app_id_str
    __app_name = app_name_str
    _pinpointPy.set_agent(collect_agent_host, trace_limit)
    if log_level == logging.DEBUG:
        def debug_func(msg: str):
            get_logger().debug(msg=msg)
        _pinpointPy.enable_debug(debug_func)
    set_logger_level(log_level)
    get_logger().debug(
        f"appid:{app_id_str} appname:{app_name_str} collector_agent:{collect_agent_host} trace_limit:{trace_limit} log_level:{log_level}")
