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



from common import *
import _pinpointPy
from  urllib.parse import urlparse


class NextSpanPlugin(PinTrace):

    def __init__(self,name):
        super().__init__(name)

    def isSample(self,args):
        '''
        if not root, no trace
        :return:
        '''
        url = args[0][0]
        target = urlparse(url).netloc
        kwargs = args[1]
        if "headers" not in kwargs:
            kwargs["headers"] = {}
        if _pinpointPy.get_context_key(PP_HEADER_PINPOINT_SAMPLED) =="s1":
            generatePinpointHeader(target, kwargs['headers'])
            return True
        else:
            kwargs['headers'][PP_HEADER_PINPOINT_SAMPLED] = PP_NOT_SAMPLED
            return False

    def onBefore(self,*args, **kwargs):
        url = args[0]
        target = urlparse(url).netloc
        super().onBefore(*args, **kwargs)
        ###############################################################
        _pinpointPy.add_clue(PP_INTERCEPTOR_NAME,self.getFuncUniqueName())
        _pinpointPy.add_clue(PP_SERVER_TYPE, PP_REMOTE_METHOD)
        _pinpointPy.add_clues(PP_ARGS, url)
        _pinpointPy.add_clues(PP_HTTP_URL, url)
        _pinpointPy.add_clue(PP_DESTINATION, target)
        ###############################################################
        return args, kwargs

    def onEnd(self,ret):
        ###############################################################
        _pinpointPy.add_clue(PP_NEXT_SPAN_ID, _pinpointPy.get_context_key(PP_NEXT_SPAN_ID))
        _pinpointPy.add_clues(PP_HTTP_STATUS_CODE, str(ret.status_code))
        _pinpointPy.add_clues(PP_RETURN, str(ret))
        ###############################################################
        super().onEnd(ret)
        return ret

    def onException(self, e):
        _pinpointPy.add_clue(PP_ADD_EXCEPTION,str(e))

    def get_arg(self, *args, **kwargs):
        args_tmp = {}
        j = 0

        for i in args:
            args_tmp["arg["+str(j)+"]"] = (str(i))
            j += 1

        for k in kwargs:
            args_tmp[k] = kwargs[k]

        return str(args_tmp)
