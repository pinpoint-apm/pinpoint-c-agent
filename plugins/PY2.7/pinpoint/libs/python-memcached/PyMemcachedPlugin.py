#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 12/3/20

from pinpoint.common import *
import pinpointPy

class PyMemcachedPlugin(Candy):
    def __init__(self, name):
        super(PyMemcachedPlugin, self).__init__(name)

    def onBefore(self, *args, **kwargs):
        super(PyMemcachedPlugin, self).onBefore(*args, **kwargs)
        pinpointPy.add_clue(PP_INTERCEPTOR_NAME,self.getFuncUniqueName())
        ###############################################################
        client = args[0]
        method = args[1]
        host = client.server
        pinpointPy.add_clue(PP_DESTINATION, host)
        pinpointPy.add_clue(PP_SERVER_TYPE, PP_MEMCACHED_FUTURE_GET)
        pinpointPy.add_clues(PP_ARGS,method)
        ###############################################################

        return args, kwargs

    def onEnd(self, ret):
        super(PyMemcachedPlugin, self).onEnd(ret)
        return ret
