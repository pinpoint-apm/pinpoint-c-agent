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


from ..common import *
from .PinpointDefine import *
import pinpointPy
from  urllib.parse import urlparse


class NextSpanPlugin(Candy):

    def __init__(self,class_name,module_name):
        super().__init__(class_name,module_name)
        self.nsid = ''
        self.url =''

    def onBefore(self,*args, **kwargs):
        args, kwargs = super().onBefore(*args, **kwargs)
        self.url = args[0]
        generatePinpointHeader(self.url, kwargs['headers'])
        ###############################################################
        pinpointPy.add_clue(FuncName,self.getFuncUniqueName())
        pinpointPy.add_clue(ServerType,PYTHON_METHOD_CALL)
        pinpointPy.add_clues(PY_ARGS, self.url)
        ###############################################################
        return args, kwargs

    def onEnd(self,ret):
        ###############################################################
        pinpointPy.add_clue("dst", urlparse(self.url)['netloc'])
        pinpointPy.add_clue("stp", PYTHON_REMOTE_METHOD)
        pinpointPy.add_clue('nsid', pinpointPy.get_context_key('nsid'))
        pinpointPy.add_clues(HTTP_URL, self.url)
        pinpointPy.add_clues(HTTP_STATUS_CODE, str(ret.status_code))
        pinpointPy.add_clues(PY_RETURN, str(ret))
        ###############################################################
        super().onEnd(ret)
        return ret

    def onException(self, e):
        pinpointPy.add_clue('EXP',str(e))

    def get_arg(self, *args, **kwargs):
        args_tmp = {}
        j = 0

        for i in args:
            args_tmp["arg["+str(j)+"]"] = (str(i))
            j += 1

        for k in kwargs:
            args_tmp[k] = kwargs[k]

        return str(args_tmp)
