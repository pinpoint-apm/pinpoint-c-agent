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

# Created by eeliu at 3/5/20

from . import Defines
from . import pinpoint

class PinTrace(object):
    E_PER_REQ=1
    E_FUNCTION = 2

    def __init__(self,name):
        self.name = name

    def isSample(self,args):
        '''
        if not root, no trace
        :return:
        '''
        if pinpoint.trace_has_root() and pinpoint.get_context(Defines.PP_HEADER_PINPOINT_SAMPLED) == "s1":
            return True
        else:
            return False

    def onBefore(self,*args, **kwargs):
        pinpoint.with_trace()

    def onEnd(self,ret):
        pinpoint.end_trace()

    def onException(self,e):
        raise NotImplementedError()

    def __call__(self, func):
        self.func_name=func.__name__
        def pinpointTrace(*args, **kwargs):
            if not self.isSample((args,kwargs)):
                return func(*args, **kwargs)

            ret = None
            try:
                args, kwargs = self.onBefore(*args, **kwargs)
                ret = func(*args, **kwargs)
                return ret
            except Exception as e:
                self.onException(e)
                raise e
            finally:
                self.onEnd(ret)
        return pinpointTrace

    def getFuncUniqueName(self):
        return self.name




