#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 1/8/21


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

#@deprecated
class ConsumerCallbackPlugin(Candy):
    def __init__(self, name):
        super().__init__(name)

    def onBefore(self,*args, **kwargs):
        return args, kwargs

    def onEnd(self, ret):
        ###############################################################

        ###############################################################
        # super().onEnd(ret)
        return ret

    def onException(self, e):
        pinpointPy.mark_as_error(str(e), "", 0)
        raise e


class AMQPConsumerPlugin(Candy):

    def __init__(self, name):
        super().__init__(name)



    def onBefore(self, *args, **kwargs):
        if 'callback' in kwargs:
            ori_callback = kwargs['callback']
            plugins = ConsumerCallbackPlugin('amqp_consumer_callback')

            @ConsumerCallbackPlugin('amqp_consumer_callback')
            def amqp_consumer_callback(message):
                # print(message.headers)
                return ori_callback(message)

            kwargs['callback'] = amqp_consumer_callback

        return args, kwargs

    def onEnd(self, ret):
        return ret

    def onException(self, e):
        raise e