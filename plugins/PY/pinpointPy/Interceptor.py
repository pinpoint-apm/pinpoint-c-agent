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

from pinpointPy import get_logger


class Interceptor:
    def __init__(self, scope, point, WrapperClass):
        assert isinstance(point, str)
        self.scope_name = scope.__name__
        self.point = point
        # new a handle and bind on full_name
        self.wrapper = WrapperClass(self.scope_name+'.'+self.point)
        self.scope = scope
        self.origin_point = getattr(scope, point)

    def enable(self):
        assert callable(self.wrapper)
        setattr(self.scope, self.point, self.wrapper(self.origin_point))
        get_logger().debug(f'enable interceptor on {self.scope}.{self.point}')

    def disable(self):
        setattr(self.scope, self.point, self.origin_point)


def intercept_once(point):
    def func(*args, **kwargs):
        if not func.already_intercept:
            func.already_intercept = True
            return point(*args, **kwargs)
    func.already_intercept = False
    return func
