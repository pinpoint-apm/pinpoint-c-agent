#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 8/20/20

from pinpoint.common import *
import pinpointPy


class MongoClientPlugin(Candy):
    def __init__(self, name):
        super().__init__(name)
        self.dst = ''

    def onBefore(self, *args, **kwargs):
        super().onBefore(*args, **kwargs)
        collection = args[0]
        self.dst = str(collection.__database.address)
        ###############################################################
        pinpointPy.add_clue(PP_INTERCEPTER_NAME, self.getFuncUniqueName())
        pinpointPy.add_clue(PP_SERVER_TYPE, PP_MONGDB_EXE_QUERY)
        # pinpointPy.add_clues(PP_ARGS, )
        ###############################################################
        pinpointPy.add_clue(PP_DESTINATION, self.dst)
        return args, kwargs

    def onEnd(self, ret):
        ###############################################################
        pinpointPy.add_clues(PP_RETURN, str(ret))
        ###############################################################
        super().onEnd(ret)
        return ret

    def onException(self, e):
        pinpointPy.add_clue(PP_ADD_EXCEPTION, str(e))
