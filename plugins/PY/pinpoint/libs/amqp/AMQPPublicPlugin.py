#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 12/29/20

from pinpoint.common import *
import pinpointPy

class AMQPPublicPlugin(Candy):
    def __init__(self, name):
        super().__init__(name)

    def onBefore(self,*args, **kwargs):
        super().onBefore(*args, **kwargs)
        ###############################################################
        pinpointPy.add_clue(PP_INTERCEPTOR_NAME, self.getFuncUniqueName())
        pinpointPy.add_clue(PP_SERVER_TYPE, PP_RABBITMQ_CLIENT)

        if 'routing_key' in kwargs and not kwargs['routing_key']:
            pinpointPy.add_clues(PP_RABBITMQ_ROUTINGKEY, kwargs['routing_key'])

        if 'exchange' in kwargs and not kwargs['exchange']:
            pinpointPy.add_clues(PP_RABBITMQ_EXCHANGEKEY, kwargs['exchange'])

        connect = args[0].connection
        pinpointPy.add_clue(PP_DESTINATION, str(connect))
        ###############################################################

        return args, kwargs

    def onEnd(self,ret):
        super().onEnd(ret)
        return ret