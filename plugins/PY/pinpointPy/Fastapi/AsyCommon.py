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



from ast import Assert
import asyncio

from starlette_context import context

from .. import pinpoint


class AsynPinTrace(object):

    def __init__(self,name):
        self.name = name

    def getCurrentId(self):
        id = context['_pinpoint_id_']
        if not id:
            raise 'not found traceId'
        else:
            return id 

    def onBefore(self,parentId,*args, **kwargs):
        traceId = pinpoint.with_trace(parentId)
        # update global id
        context['_pinpoint_id_'] = traceId
        return traceId,args,kwargs

    @staticmethod
    def isSample(*args, **kwargs):
        try:
            parentid = context.get('_pinpoint_id_',0)
            if parentid == 0:
                return False,None
            return True,parentid
        except Exception as e:
            return False,None

    @classmethod
    def _isSample(cls,*args, **kwargs):
        return cls.isSample(*args, **kwargs)

    def onEnd(self,parentId,ret):
        parentId = pinpoint.end_trace(parentId)
        context['_pinpoint_id_'] = parentId

    def onException(self,traceId,e):
        raise NotImplementedError()
    
    def __call__(self, func):
        self.func_name=func.__name__

        async def pinpointTrace(*args, **kwargs):
            ret = None
            sampled,parentId = self._isSample(args, kwargs)
            if not sampled:
                return await func(*args, **kwargs)

            traceId,args,kwargs = self.onBefore(parentId,*args, **kwargs)
            try:
                ret = await func(*args, **kwargs)
                return ret
            except Exception as e:
                self.onException(traceId,e)
                raise e
            finally:
                self.onEnd(traceId,ret)

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