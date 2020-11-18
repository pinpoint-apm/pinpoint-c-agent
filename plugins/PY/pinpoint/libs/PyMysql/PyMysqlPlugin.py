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


from pinpoint.common import *
import pinpointPy

class PyMysqlPlugin(Candy):

    def __init__(self,name):
        super().__init__(name)

    def onBefore(self,*args, **kwargs):
        super().onBefore(*args, **kwargs)
        ###############################################################
        pinpointPy.add_clue(PP_INTERCEPTER_NAME,self.getFuncUniqueName())
        pinpointPy.add_clue(PP_SERVER_TYPE, PP_MYSQL)
        pinpointPy.add_clue(PP_SQL_FORMAT,  args[1])
        ###############################################################
        cursor = args[0]
        dst = cursor.connection.get_host_info()
        pinpointPy.add_clue(PP_DESTINATION, dst)
        return args,kwargs

    def onEnd(self,ret):
        super().onEnd(ret)
        return ret

    def onException(self, e):
        pinpointPy.add_clue(PP_ADD_EXCEPTION,str(e))

    def get_arg(self, *args, **kwargs):
        args_tmp = {}
        j = 0

        for i in args:
            args_tmp["arg["+str(j)+"]"] = (str(i))
            j += 1
        # print(str(args_tmp))
        for k in kwargs:
            args_tmp[k] = kwargs[k]
        # print(str(args_tmp))
        return str(args_tmp)
