#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 11/11/20

from pinpoint.common import *


class AioRequestPlugin(AsyRequestPlugin):
    def onBefore(self,*args, **kwargs):
        super().onBefore(*args, **kwargs)
        request = args[0]
        pinpointPy.add_clue(PP_INTERCEPTER_NAME, 'aiohttp-middleware',self.traceId)
        pinpointPy.add_clue(PP_REQ_URI, request.path, self.traceId)
        pinpointPy.add_clue(PP_REQ_CLIENT, request.remote, self.traceId)
        pinpointPy.add_clue(PP_REQ_SERVER, request.host, self.traceId)


    def onEnd(self, ret):
        response  = ret
        return super().onEnd(response)
