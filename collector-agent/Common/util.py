#!/usr/bin/env python
# -*- coding: UTF-8 -*-# Created by eeliu at 1/14/20

# ------------------------------------------------------------------------------
#  Copyright  2020. NAVER Corp.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
# ------------------------------------------------------------------------------
import time

from Common.Logger import TCLogger


def profile(func):
    def _profile_func(*args, **kwargs):
        _start = time.time()
        ret = func(*args, **kwargs)
        _takes = int(time.time() - _start)
        if _takes > 3:
            TCLogger.warning("A slow func:%s takes:%d sec", func.__name__, _takes)
        return ret

    return _profile_func
