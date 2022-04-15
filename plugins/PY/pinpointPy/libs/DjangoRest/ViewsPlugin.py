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


from pinpointPy.common import *
import _pinpointPy

class ViewsPlugin(PinTrace):

    def __init__(self,name):
        super().__init__(name)

    def onBefore(self,*args, **kwargs):
        super().onBefore(*args, **kwargs)
        ###############################################################
        _pinpointPy.add_clue(PP_INTERCEPTOR_NAME,args[0].get_view_name())
        _pinpointPy.add_clue(PP_SERVER_TYPE, PP_METHOD_CALL)
        ###############################################################
        url = '{%s %r}' % (args[1].method,args[1].get_full_path())
        _pinpointPy.add_clues(PP_ARGS, url)
        return args,kwargs

    def onEnd(self,ret):
        super().onEnd(ret)
        return ret

    def onException(self, e):
        _pinpointPy.add_clue(PP_ADD_EXCEPTION,str(e))
