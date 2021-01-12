#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 12/29/20


import pinpointPy

from pinpoint.common import PinpointCommonPlugin, PP_DESTINATION, PP_NEXT_SPAN_ID, generateSid, \
    PP_SERVER_TYPE, PP_METHOD_CALL, PP_REMOTE_METHOD


class TaskPlugin(PinpointCommonPlugin):
    def onBefore(self, *args, **kwargs):
        super().onBefore(*args, **kwargs)
        pinpointPy.add_clue(PP_SERVER_TYPE, PP_REMOTE_METHOD)
        pinpointPy.add_clue(PP_DESTINATION,'test-virtual-machine')
        nsid = generateSid()
        pinpointPy.set_context_key(PP_NEXT_SPAN_ID, nsid)
        pinpointPy.add_clue(PP_NEXT_SPAN_ID,nsid)
        print(args[0].name)
        return args,kwargs




