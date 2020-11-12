#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 11/12/20


from pinpoint.settings import *
from pinpoint.common import *

class PyRequestPlugin(RequestPlugin):
    def onBefore(self,*args, **kwargs):
        super().onBefore(*args, **kwargs)
        request = args[0]
        pinpointPy.add_clue(PP_INTERCEPTER_NAME, 'Pyramid-middleware')
        pinpointPy.add_clue(PP_REQ_URI, request.path)
        pinpointPy.add_clue(PP_REQ_CLIENT, request.remote_addr)
        pinpointPy.add_clue(PP_REQ_SERVER, request.host)


    def onEnd(self, ret):
        response  = ret
        return super().onEnd(response)
