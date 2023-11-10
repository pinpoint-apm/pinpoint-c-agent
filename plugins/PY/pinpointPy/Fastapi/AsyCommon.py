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
from starlette_context import context
from pinpointPy.TraceContext import TraceContext
from pinpointPy.Common import PinTrace

class AsyncTraceContext (TraceContext):
    def get_parent_id(self):
        id = context.get('_pinpoint_id_', 0)
        if id == 0:
            return False, -1
        else:
            return True, id

    def set_parent_id(self, id: int):
        context['_pinpoint_id_'] = id


class AsyncPinTrace(PinTrace):
    
    def __call__(self, func):
        self.func_name = func.__name__

        async def pinpointTrace(*args, **kwargs):
            ret = None
            # avoiding variable missing
            # use and return
            sampled, parentId, nArgs, nKwargs = self._isSample(*args, **kwargs)
            if not sampled:
                return await func(*nArgs, **nKwargs)
            traceId, nArgs, nKwargs = self.onBefore(
                parentId, *nArgs, **nKwargs)
            try:
                ret = await func(*nArgs, **nKwargs)
                return ret
            except Exception as e:
                self.onException(traceId, e)
                raise e
            finally:
                self.onEnd(traceId, ret)
        return pinpointTrace
