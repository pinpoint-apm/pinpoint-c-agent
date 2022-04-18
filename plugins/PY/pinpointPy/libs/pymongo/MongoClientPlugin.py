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

from pinpointPy.common import *
import _pinpointPy


class MongoClientPlugin(PinTrace):
    def __init__(self, name):
        super().__init__(name)
        self.dst = ''

    def onBefore(self, *args, **kwargs):
        super().onBefore(*args, **kwargs)
        collection = args[0]
        self.dst = str(collection.__database.address)
        ###############################################################
        _pinpointPy.add_clue(PP_INTERCEPTOR_NAME, self.getFuncUniqueName())
        _pinpointPy.add_clue(PP_SERVER_TYPE, PP_MONGDB_EXE_QUERY)
        # _pinpointPy.add_clues(PP_ARGS, )
        ###############################################################
        _pinpointPy.add_clue(PP_DESTINATION, self.dst)
        return args, kwargs

    def onEnd(self, ret):
        ###############################################################
        _pinpointPy.add_clues(PP_RETURN, str(ret))
        ###############################################################
        super().onEnd(ret)
        return ret

    def onException(self, e):
        _pinpointPy.add_clue(PP_ADD_EXCEPTION, str(e))
