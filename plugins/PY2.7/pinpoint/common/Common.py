# ******************************************************************************
#   Copyright  2020. NAVER Corp.
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
# ******************************************************************************

import random
import pinpointPy
from pinpoint.common.Defines import *
from pinpoint.settings import *

class Candy(object):
    def __init__(self,name):
        self.name = name

    def onBefore(self,*args, **kwargs):
        pinpointPy.start_trace()

    def onEnd(self,ret):
        pinpointPy.end_trace()

    def onException(self,e):
        pinpointPy.add_clue(PP_ADD_EXCEPTION, str(e))

    def __call__(self, func):
        self.func_name=func.__name__
        def pinpointTrace(*args, **kwargs):
            ret = None
            self.onBefore(*args, **kwargs)
            try:
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