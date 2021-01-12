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

# Created by eeliu at 8/20/20

import pinpointPy
from pinpoint.common import *


class MongoClientPlugin(AsyCandy):
    def __init__(self, name):
        super().__init__(name)
        self.dst = ''


    def onBefore(self, *args, **kwargs):
        super().onBefore(*args, **kwargs)
        self.dst = str(args[0].database._client.address)
        # ###############################################################
        pinpointPy.add_clue(PP_INTERCEPTOR_NAME, self.getFuncUniqueName(),self.traceId)
        pinpointPy.add_clue(PP_SERVER_TYPE, PP_MONGDB_EXE_QUERY,self.traceId)
        # ###############################################################
        pinpointPy.add_clue(PP_DESTINATION, self.dst,self.traceId)
        return args, kwargs

    def onEnd(self, ret):
        ###############################################################
        # pinpointPy.add_clues(PP_RETURN, str(ret),self.traceId)
        ###############################################################
        super().onEnd(ret)
        return ret

    def onException(self, e):
        pinpointPy.add_clue(PP_ADD_EXCEPTION, str(e),self.traceId)

    def __call__(self, func):
        self.func_name=self.getFuncUniqueName()
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
