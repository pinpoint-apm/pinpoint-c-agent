#!/usr/bin/env python
# -*- coding: UTF-8 -*-

# Created by eeliu at 11/23/24


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

from pinpointPy import Common, pinpoint, Defines


class Plugin(Common.PinTrace):

    def __init__(self, name):
        super().__init__(name)

    def get_host(self, *args, **kwargs):
        inst = args[0]
        from amqp import Channel
        if isinstance(inst, Channel):
            con = inst.connection
            return con.host
        else:
            return "localhost"

    def get_routing_exchange(self, *args, **kwargs):
        if kwargs:
            return "" if 'routing_key' not in kwargs else kwargs['routing_key'], "" if 'exchange' not in kwargs else kwargs['exchange']
        else:
            return "", ""

    def onBefore(self, parentId, *args, **kwargs):
        trace_id, args, kwargs = super().onBefore(parentId, *args, **kwargs)
        pinpoint.add_trace_header(
            Defines.PP_INTERCEPTOR_NAME, self.getUniqueName(), trace_id)
        pinpoint.add_trace_header(
            Defines.PP_SERVER_TYPE, Defines.PP_RABBITMQ_CLIENT, trace_id)

        pinpoint.add_trace_header(
            Defines.PP_DESTINATION, self.get_host(*args, **kwargs), trace_id)

        routing, ex = self.get_routing_exchange(*args, **kwargs)
        pinpoint.add_trace_header_v2(
            Defines.PP_RABBITMQ_ROUTINGKEY, routing, trace_id)
        pinpoint.add_trace_header_v2(
            Defines.PP_RABBITMQ_EXCHANGEKEY, ex, trace_id)

        return trace_id, args, kwargs

    def onEnd(self, traceId, ret):
        super().onEnd(traceId, ret)
        return ret

    def onException(self, traceId, e):
        pinpoint.add_exception(str(e), traceId)
