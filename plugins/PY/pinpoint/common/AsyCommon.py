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



import asyncio
import pinpointPy
import contextvars

_Id_ = contextvars.ContextVar('_pinpoint_id_',default=0)

class AsyCandy(object):

    def __init__(self,name):
        self.name = name
        self.traceId = 0

    def onBefore(self,*args, **kwargs):
        global _Id_
        id = _Id_.get()
        traceId = pinpointPy.start_trace(id)
        _Id_.set(traceId)
        self.traceId = traceId
        return (args,kwargs)

    def onEnd(self,ret):
        traceId = pinpointPy.end_trace(self.traceId)
        global _Id_
        _Id_.set(traceId)

    def onException(self,e):
        raise NotImplementedError()

    def __call__(self, func):
        self.func_name=func.__name__
        async def pinpointTrace(*args, **kwargs):
            ret = None
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

    @AsyCandy('main')
    async def run(i):
        if i == 0:
            return
        print("run")
        await asyncio.sleep(0.1)
        await run(i-1)

    asyncio.run(run(2))
    asyncio.run(run(2))