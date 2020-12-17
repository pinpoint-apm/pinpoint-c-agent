#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 12/3/20

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
