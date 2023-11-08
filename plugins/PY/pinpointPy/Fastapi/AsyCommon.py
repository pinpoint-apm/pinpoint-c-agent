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
import asyncio

from starlette_context import context

from pinpointPy import pinpoint


class TraceContext:
    @staticmethod
    def get_parent_id():
        id = context.get('_pinpoint_id_', 0)
        if id == 0:
            return False, None
        else:
            return True, id

    @staticmethod
    def set_parent_id(id: int):
        context['_pinpoint_id_'] = id


class AsyncPinTrace:

    def __init__(self, name):
        self.name = name

    def onBefore(self, parentId, *args, **kwargs):
        traceId = pinpoint.with_trace(parentId)
        TraceContext.set_parent_id(traceId)
        return traceId, args, kwargs

    @staticmethod
    def isSample(*args, **kwargs):
        ret, id = TraceContext.get_parent_id()
        if ret:
            return True, id, args, kwargs
        else:
            return False, None, args, kwargs

    @classmethod
    def _isSample(cls, *args, **kwargs):
        return cls.isSample(*args, **kwargs)

    def onEnd(self, parentId, ret):
        parentId = pinpoint.end_trace(parentId)
        TraceContext.set_parent_id(parentId)

    def onException(self, traceId, e):
        raise NotImplementedError()

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

    def getFuncUniqueName(self):
        return self.name


if __name__ == '__main__':

    @AsyncPinTrace('main')
    async def run(i):
        if i == 0:
            return
        print("run")
        await asyncio.sleep(0.1)
        await run(i-1)

    asyncio.run(run(2))
    asyncio.run(run(2))
