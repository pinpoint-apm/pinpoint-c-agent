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



import random
import asyncio
from ..settings import *
from  ..plugins.common.PinpointDefine import *
import pinpointPy
import contextvars

pinpointId = contextvars.ContextVar('_pinpoint_id_')
pinpointId.set(0)


class AsyCandy(object):

    def __init__(self,class_name,module_name):
        self.class_name = class_name
        self.module_name = module_name
        self.node_id = 0

    def onBefore(self,*args, **kwargs):
        id = pinpointId.get()
        new_id = pinpointPy.start_trace(id)
        pinpointId.set(new_id)
        self.node_id = new_id
        return (args,kwargs)

    def onEnd(self,ret):
        new_id = pinpointPy.end_trace(self.node_id)
        pinpointId.set(new_id)

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
                # print("end", self.func_name)
                self.onEnd(ret)

        return pinpointTrace

    def generateTid(self):
        return ('%s^%s^%s') % (APP_ID,str(pinpointPy.start_time()), str(pinpointPy.unique_id()))

    def generateSid(self):
        return str(random.randint(0,2147483647))

    def getFuncUniqueName(self):
        if self.class_name:
            return '%s\%s.%s'%(self.module_name,self.class_name,self.func_name)
        else:
            return '%s\%s'%(self.module_name,self.func_name)

if __name__ == '__main__':

    @AsyCandy('main',__name__)
    async def run(i):
        if i == 0:
            return
        print("run")
        await asyncio.sleep(0.1)
        await run(i-1)

    asyncio.run(run(2))
    asyncio.run(run(2))