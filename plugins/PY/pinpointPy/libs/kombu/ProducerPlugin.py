#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 12/29/20


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

from pinpointPy.common import *
import _pinpointPy

class ProducerPlugin(PinTrace):
    def __init__(self, name):
        super().__init__(name)
    
    def onBefore(self,*args, **kwargs):
        super().onBefore(*args, **kwargs)
        ###############################################################
        _pinpointPy.add_clue(PP_INTERCEPTOR_NAME, self.getFuncUniqueName())
        _pinpointPy.add_clue(PP_SERVER_TYPE, PP_RABBITMQ_CLIENT)
        if 'exchange' in kwargs and not kwargs['exchange']:
            _pinpointPy.add_clues(PP_RABBITMQ_EXCHANGEKEY, kwargs['exchange'])

        if 'routing_key' in kwargs and not kwargs['routing_key']:
            if kwargs['routing_key'] == 'worker.heartbeat':
                _pinpointPy.drop_trace()
            _pinpointPy.add_clues(PP_RABBITMQ_ROUTINGKEY, kwargs['routing_key'])
        
        if 'task' in  kwargs['headers']:
            target = kwargs['headers']['task']
            generatePPRabbitMqHeader(target,kwargs['headers'])

        uri = args[0].connection.as_uri()
        _pinpointPy.add_clue(PP_DESTINATION, str(uri))
        ###############################################################
        return args, kwargs

    def onEnd(self,ret):
        super().onEnd(ret)
        return ret