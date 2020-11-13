#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 11/9/20

from bottle import response

from pinpoint.libs import monkey_patch_for_pinpoint
monkey_patch_for_pinpoint()

import pinpointPy

class RequestPlugin(WSGIPlugin):

    def onEnd(self, ret):
        pinpointPy.add_clues(PP_HTTP_STATUS_CODE,str(response.status_code))
        if response.status_code !=200:
            pinpointPy.add_clue(PP_ADD_EXCEPTION,"status code not 200")
        return super().onEnd(ret)