#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 12/29/20

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