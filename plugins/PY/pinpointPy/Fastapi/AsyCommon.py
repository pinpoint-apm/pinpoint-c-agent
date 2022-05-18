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

from .. import pinpoint


class AsynPinTrace(object):

    def __init__(self,name):
        self.name = name
        self.traceId = -1

    def onBefore(self,*args, **kwargs):
        id = context.get('_pinpoint_id_', default=0)
        traceId = pinpoint.with_trace(id)
        context['_pinpoint_id_'] = traceId
        self.traceId = traceId
        return (args,kwargs)

    def isSample(args,kwargs):
        try:
            context.get('_pinpoint_id_', default=0)
            return True
        except Exception as e:
            return False

    def onEnd(self,ret):
        traceId = pinpoint.end_trace(self.traceId)
        context['_pinpoint_id_'] = traceId

    def onException(self,e):
        raise NotImplementedError()
    
    def __call__(self, func):
        self.func_name=func.__name__

        async def pinpointTrace(*args, **kwargs):
            ret = None
            if not self.isSample((args,kwargs)):
                return await func(*args, **kwargs)

            self.onBefore(*args, **kwargs)
            try:
                ret = await func(*args, **kwargs)
                return ret
            except Exception as e:
                self.onException(e)
                raise e
            finally:
                self.onEnd(ret)

        return pinpointTrace

    def getFuncUniqueName(self):
        return self.name

if __name__ == '__main__':

    @AsynPinTrace('main')
    async def run(i):
        if i == 0:
            return
        print("run")
        await asyncio.sleep(0.1)
        await run(i-1)

    asyncio.run(run(2))
    asyncio.run(run(2))