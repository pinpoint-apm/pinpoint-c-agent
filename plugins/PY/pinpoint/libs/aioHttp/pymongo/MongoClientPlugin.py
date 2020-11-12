#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 8/20/20

import pinpointPy
from pinpoint.common import *


class MongoClientPlugin(AsyCandy):
    def __init__(self, name):
        super().__init__(name)
        self.dst = ''


    def onBefore(self, *args, **kwargs):
        super().onBefore(*args, **kwargs)
        self.dst = str(args[0].database._client.address)
        # ###############################################################
        pinpointPy.add_clue(PP_INTERCEPTER_NAME, self.getFuncUniqueName(),self.traceId)
        pinpointPy.add_clue(PP_SERVER_TYPE, PP_MONGDB_EXE_QUERY,self.traceId)
        print(args[0].database._client.address)
        # ###############################################################
        pinpointPy.add_clue(PP_DESTINATION, self.dst,self.traceId)
        print("------------------")
        return args, kwargs

    def onEnd(self, ret):
        ###############################################################
        # pinpointPy.add_clues(PP_RETURN, str(ret),self.traceId)
        ###############################################################
        super().onEnd(ret)
        return ret

    def onException(self, e):
        pinpointPy.add_clue(PP_ADD_EXCEPTION, str(e),self.traceId)

    def __call__(self, func):
        self.func_name=self.getFuncUniqueName()
        def pinpointTrace(*args, **kwargs):
            ret = None
            self.onBefore(*args, **kwargs)
            try:
                ret = func(*args, **kwargs)
                return ret
            except Exception as e:
                self.onException(e)
                raise e
            finally:
                self.onEnd(ret)

        return pinpointTrace
