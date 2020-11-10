#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 3/5/20

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

import pinpointPy

class Candy(object):
    def __init__(self,name):
        self.name = name


    def onBefore(self,*args, **kwargs):
        pinpointPy.start_trace()
        return (args,kwargs)

    def onEnd(self,ret):
        pinpointPy.end_trace()

    def onException(self,e):
        raise NotImplementedError()

    def __call__(self, func):
        self.func_name=func.__name__
        def pinpointTrace(*args, **kwargs):
            ret = None
            args, kwargs = self.onBefore(*args, **kwargs)
            try:
                ret = func(*args, **kwargs)
                return ret
            except Exception as e:
                self.onException(e)
                raise e
            finally:
                # print("end", self.func_name)
                self.onEnd(ret)

        return pinpointTrace

    def getFuncUniqueName(self):
        return self.name

if __name__ == '__main__':

    @Candy('main')
    def run():
        print("run")

    run()

    run()
