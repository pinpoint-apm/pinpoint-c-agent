#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 12/3/20

from pinpoint.common import *
import pinpointPy

class AmqpPlugin(Candy):
    def __init__(self, name):
        super(AmqpPlugin, self).__init__(name)

    def onBefore(self, *args, **kwargs):
        super(AmqpPlugin, self).onBefore(*args, **kwargs)
        pinpointPy.add_clue(PP_INTERCEPTOR_NAME,self.getFuncUniqueName())
        ###############################################################
        channel =args[0]
        host = channel.connection.client.host
        pinpointPy.add_clue(PP_DESTINATION, host)
        pinpointPy.add_clue(PP_SERVER_TYPE, PP_RABBITMQ_CLIENT)
        pinpointPy.add_clues(PP_RABBITMQ_ROUTINGKEY,kwargs['routing_key'])
        ###############################################################

        return args, kwargs

    def onEnd(self, ret):
        super(AmqpPlugin, self).onEnd(ret)
        return ret
