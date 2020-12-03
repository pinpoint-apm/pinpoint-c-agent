#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 12/3/20

from pinpoint.common import *
import pinpointPy

class PymemcachePlugin(Candy):
    def __init__(self, name):
        super(PymemcachePlugin, self).__init__(name)

    def onBefore(self, *args, **kwargs):
        super(PymemcachePlugin, self).onBefore(*args, **kwargs)
        pinpointPy.add_clue(PP_INTERCEPTOR_NAME,self.getFuncUniqueName())
        ###############################################################
        client = args[0]
        cmd = args[1]
        pinpointPy.add_clue(PP_DESTINATION, self._ClientServerStr(client.servers))
        pinpointPy.add_clues(PP_ARGS,cmd)
        ###############################################################
        pinpointPy.add_clue(PP_SERVER_TYPE, PP_MEMCACHED_FUTURE_GET)

        return args, kwargs

    def onEnd(self, ret):
        super(PymemcachePlugin, self).onEnd(ret)
        return ret
    def _ClientServerStr(self,servers):
        _server = ''
        for s in servers:
            _server+='['
            _server+=s
            _server+=']'
        return _server
