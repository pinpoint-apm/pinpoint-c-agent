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

# Created by eeliu at 11/13/2024

from pinpointPy import Common, pinpoint, Defines
import kombu


class KombuPlugin(Common.PinTrace):

    def __init__(self, name):
        super().__init__(name)

    def get_host(self, *args, **kwargs):
        message = args[0]
        from kombu.messaging import Producer
        if isinstance(message, Producer):
            return message.connection.host
        else:
            return "localhost"

    def get_routing_exchange(self, *args, **kwargs):
        if kwargs:
            # return "" if 'routing_key' not in kwargs else kwargs['routing_key'], "" if 'exchange' not in kwargs else kwargs['exchange'].name
            routing_key = "" if 'routing_key' not in kwargs else kwargs['routing_key']
            exchange = "" if 'exchange' not in kwargs else kwargs['exchange']

            if isinstance(exchange, kombu.entity.Exchange):
                exchange = exchange.name
            return routing_key, exchange
        else:
            return "", ""

    def onBefore(self, parentId, *args, **kwargs):
        trace_id, args, kwargs = super().onBefore(parentId, *args, **kwargs)
        pinpoint.add_trace_header(
            Defines.PP_INTERCEPTOR_NAME, self.getUniqueName(), trace_id)
        # fixme treat all type as rabbitmq
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
