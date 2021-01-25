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
        print(args[0])
        return args, kwargs

    # def onBefore(self, *args, **kwargs):
    #     super().onBefore(*args, **kwargs)
    #     pinpointPy.add_clue(PP_APP_NAME, APP_NAME)
    #     pinpointPy.add_clue(PP_APP_ID, APP_ID)
    #     pinpointPy.add_clue(PP_AGENT_TYPE,PP_CELERY_WORKER)
    #     pinpointPy.add_clue(PP_SERVER_TYPE, PP_CELERY_WORKER)
    #     pinpointPy.add_clue(PP_INTERCEPTOR_NAME, self.getFuncUniqueName())
    #
    #     #############################################################
    #     message = args[0]
    #     headers = {} if message.headers is None else message.headers
    #
    #     pinpointPy.set_context_key(PP_HEADER_PINPOINT_SAMPLED, "s1")
    #
    #     if PP_REQ_CLIENT in headers:
    #         pinpointPy.add_clue(PP_REQ_CLIENT, headers[PP_REQ_CLIENT])
    #
    #     if PP_REQ_SERVER in headers:
    #         pinpointPy.add_clue(PP_REQ_SERVER, headers[PP_REQ_SERVER])
    #
    #     if PP_HTTP_PINPOINT_PSPANID in headers:
    #         pinpointPy.add_clue(PP_PARENT_SPAN_ID, headers[PP_HTTP_PINPOINT_PSPANID])
    #
    #     if PP_HTTP_PINPOINT_SPANID in headers:
    #         self.sid = headers[PP_HTTP_PINPOINT_SPANID]
    #     elif PP_HEADER_PINPOINT_SPANID in headers:
    #         self.sid = headers[PP_HEADER_PINPOINT_SPANID]
    #     else:
    #         self.sid = generateSid()
    #     pinpointPy.set_context_key(PP_SPAN_ID, self.sid)
    #
    #     if PP_HTTP_PINPOINT_TRACEID in headers:
    #         self.tid = headers[PP_HTTP_PINPOINT_TRACEID]
    #     elif PP_HEADER_PINPOINT_TRACEID in headers:
    #         self.tid = headers[PP_HEADER_PINPOINT_TRACEID]
    #     else:
    #         self.tid = generateTid()
    #     pinpointPy.set_context_key(PP_TRANSCATION_ID, self.tid)
    #
    #     if PP_HTTP_PINPOINT_PAPPNAME in headers:
    #         self.pname = headers[PP_HTTP_PINPOINT_PAPPNAME]
    #         pinpointPy.set_context_key(PP_PARENT_NAME, self.pname)
    #         pinpointPy.add_clue(PP_PARENT_NAME, self.pname)
    #
    #     if PP_HTTP_PINPOINT_PAPPTYPE in headers:
    #         self.ptype = headers[PP_HTTP_PINPOINT_PAPPTYPE]
    #         pinpointPy.set_context_key(PP_PARENT_TYPE, self.ptype)
    #         pinpointPy.add_clue(PP_PARENT_TYPE, self.ptype)
    #
    #     if PP_HTTP_PINPOINT_HOST in headers:
    #         self.Ah = headers[PP_HTTP_PINPOINT_HOST]
    #         pinpointPy.set_context_key(PP_PARENT_HOST, self.Ah)
    #         pinpointPy.add_clue(PP_PARENT_HOST, self.Ah)
    #
    #     if PP_HTTP_PINPOINT_PSPANID in headers:
    #         pinpointPy.add_clue(PP_PARENT_SPAN_ID, headers[PP_HTTP_PINPOINT_PSPANID])
    #
    #     if PP_HEADER_PINPOINT_PAPPNAME in headers:
    #         self.pname = headers[PP_HEADER_PINPOINT_PAPPNAME]
    #         pinpointPy.set_context_key(PP_PARENT_NAME, self.pname)
    #         pinpointPy.add_clue(PP_PARENT_NAME, self.pname)
    #
    #     if PP_HEADER_PINPOINT_PAPPTYPE in headers:
    #         self.ptype = headers[PP_HEADER_PINPOINT_PAPPTYPE]
    #         pinpointPy.set_context_key(PP_PARENT_TYPE, self.ptype)
    #         pinpointPy.add_clue(PP_PARENT_TYPE, self.ptype)
    #
    #     if PP_HEADER_PINPOINT_HOST in headers:
    #         self.Ah = headers[PP_HEADER_PINPOINT_HOST]
    #         pinpointPy.set_context_key(PP_PARENT_HOST, self.Ah)
    #         pinpointPy.add_clue(PP_PARENT_HOST, self.Ah)
    #
    #     if (PP_HTTP_PINPOINT_SAMPLED in headers and headers[PP_HTTP_PINPOINT_SAMPLED] == PP_NOT_SAMPLED) \
    #             or pinpointPy.check_tracelimit():
    #         pinpointPy.drop_trace()
    #         pinpointPy.set_context_key(PP_HEADER_PINPOINT_SAMPLED, "s0")
    #
    #     pinpointPy.add_clue(PP_TRANSCATION_ID,self.tid)
    #     pinpointPy.add_clue(PP_SPAN_ID,self.sid)
    #     pinpointPy.set_context_key(PP_TRANSCATION_ID, self.tid)
    #     pinpointPy.set_context_key(PP_SPAN_ID, self.sid)
    #
    #     # todo get the callname
    #     # pinpointPy.add_clues(PP_HTTP_METHOD,headers["REQUEST_METHOD"])
    #
    #     #############################################################
    #     return args, kwargs

    def onEnd(self, ret):
        ###############################################################

        ###############################################################
        # super().onEnd(ret)
        return ret

    def onException(self, e):
        pinpointPy.mark_as_error(str(e), "", 0)
        raise e

# AMQPConsumerPlugin is the entry for consumer-callback trace
#
# ch.basic_consumer(queue='xxx',callback=xxxx)
#

class AMQPConsumerPlugin(Candy):

    def __init__(self, name):
        super().__init__(name)

    def onBefore(self, *args, **kwargs):
        if 'callback' in kwargs:
            ori_callback = kwargs['callback']
            plugins = ConsumerCallbackPlugin('amqp_consumer_callback')

            @ConsumerCallbackPlugin('amqp_consumer_callback')
            def amqp_consumer_callback(message):
                print('Received message (delivery tag: {}): {}'.format(message.delivery_tag, message.body))
                # print(message.headers)
                return ori_callback(message)


            kwargs['callback'] = amqp_consumer_callback

        return args, kwargs

    def onEnd(self, ret):
        return ret

    def onException(self, e):
        raise e