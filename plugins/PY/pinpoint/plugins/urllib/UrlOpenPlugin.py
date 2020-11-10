#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 8/20/20

from ..common import *
import pinpointPy

class UrlOpenPlugin(Candy):

    def __init__(self, name):
        super().__init__(name)
        self.dst = ''
        self.url =''
    def onBefore(self,*args, **kwargs):
        args, kwargs = super().onBefore(*args, **kwargs)
        self.url = args[0]
        generatePinpointHeader(self.url,kwargs['headers'])
        ###############################################################
        pinpointPy.add_clue(PP_INTERCEPTER_NAME,self.getFuncUniqueName())
        pinpointPy.add_clue(PP_SERVER_TYPE,PYTHON_METHOD_CALL)
        pinpointPy.add_clues(PP_ARGS, self.url)
        ###############################################################

        return args,kwargs

    def onEnd(self,ret):
        ###############################################################
        pinpointPy.add_clue("dst", urlparse(self.url)['netloc'])
        pinpointPy.add_clue("stp", PYTHON_REMOTE_METHOD)
        pinpointPy.add_clue(PP_NEXT_SPAN_ID, pinpointPy.get_context_key(PP_NEXT_SPAN_ID))
        pinpointPy.add_clues(PP_HTTP_URL, self.url)
        pinpointPy.add_clues(PP_HTTP_STATUS_CODE, str(ret.status_code))
        pinpointPy.add_clues(PY_RETURN, str(ret))
        ###############################################################
        super().onEnd(ret)
        return ret

    def onException(self, e):
        pinpointPy.add_clue('EXP',str(e))

