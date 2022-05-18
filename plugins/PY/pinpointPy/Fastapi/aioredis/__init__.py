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

# Created by suwei at 8/20/20

from ...Interceptor import Interceptor,intercept_once

@intercept_once
def monkey_patch():
    from .AioRedisPlugins import AioRedisPlugins
    try:
        from aioredis.client import Redis
        Interceptors = [
            Interceptor(Redis, 'execute_command', AioRedisPlugins)
        ]

        for interceptor in Interceptors:
            interceptor.enable()

    except ImportError as e:
        # do nothing
        print(e)

__all__=['monkey_patch']