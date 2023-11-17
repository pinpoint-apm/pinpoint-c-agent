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

from pinpointPy import Common
from pinpointPy import pinpoint
from pinpointPy import Defines


class MongoClientPlugin(Common.PinTrace):
    def __init__(self, name):
        super().__init__(name)

    def onBefore(self, parentId, *args, **kwargs):
        traceId, args, kwargs = super().onBefore(parentId, *args, **kwargs)
        collection = args[0]
        try:
            dst = str(collection.__database.address)
        except AttributeError:
            dst = collection.name
        pinpoint.add_trace_header(
            Defines.PP_INTERCEPTOR_NAME, self.getUniqueName(), traceId)
        pinpoint.add_trace_header(
            Defines.PP_SERVER_TYPE, Defines.PP_MONGDB_EXE_QUERY, traceId)
        pinpoint.add_trace_header(Defines.PP_DESTINATION, dst, traceId)
        return traceId, args, kwargs

    def onEnd(self, traceId, ret):
        # disable PP_RETURN
        # unreadable format
        # pymongo.cursor.Cursor object at 0x7f9dc76adaf0
        # InsertManyResult([ObjectId('6553441f7ef332013afc40a2'), ObjectId('6553441f7ef332013afc40a3')], acknowledged=True)
        # pinpoint.add_trace_header_v2(Defines.PP_RETURN, str(ret), traceId)
        super().onEnd(traceId, ret)
        return ret

    def onException(self, traceId, e):
        pinpoint.add_trace_header(Defines.PP_ADD_EXCEPTION, str(e), traceId)
