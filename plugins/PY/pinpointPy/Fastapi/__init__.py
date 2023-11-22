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

import importlib
import warnings

from pinpointPy.Fastapi.PinTranscation import PinTransaction, PinStarlettePlugin
from pinpointPy.Fastapi.AsyCommonPlugin import CommonPlugin
from pinpointPy.Fastapi.AsyCommon import AsyncTraceContext
from pinpointPy.Fastapi.middleware import PinPointMiddleWare
from pinpointPy.Common import PinHeader, GenPinHeader
from pinpointPy.pinpoint import get_logger
from pinpointPy.TraceContext import set_trace_context


def __monkey_patch(*args, **kwargs):
    for key in kwargs:
        if kwargs[key]:
            module = importlib.import_module('pinpointPy.Fastapi.' + key)
            monkey_patch = getattr(module, 'monkey_patch')
            if callable(monkey_patch):
                get_logger().debug(
                    f"try to install pinpointPy.Fastapi.{key} module")
                monkey_patch()


def async_monkey_patch_for_pinpoint(AioRedis=True, httpx=True):
    __monkey_patch(_aioredis=AioRedis, _httpx=httpx)


def asyn_monkey_patch_for_pinpoint(
        AioRedis=True, httpx=True):
    warnings.warn(
        "deprecated try to use `async_monkey_patch_for_pinpoint`", DeprecationWarning)
    async_monkey_patch_for_pinpoint(AioRedis, httpx)


def use_starlette_context():
    set_trace_context(new_trace_context=AsyncTraceContext())


__version__ = '0.0.2'
__author__ = 'liu.mingyi@navercorp.com'
__all__ = ['async_monkey_patch_for_pinpoint', 'asyn_monkey_patch_for_pinpoint', 'use_starlette_context', 'PinPointMiddleWare',
           'CommonPlugin', 'PinTransaction', 'PinHeader', 'GenPinHeader', 'PinStarlettePlugin']
