#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 8/20/20

from ..common import *
import pinpointPy


class MongoClientPlugin(Candy):
    def __init__(self,class_name,module_name=None):
        super().__init__(class_name, module_name)
        self.dst = ''

    def onBefore(self, *args, **kwargs):
        args, kwargs = super().onBefore(*args, **kwargs)
        collection = args[0]
        self.dst = str(collection.__database.address)
        ###############################################################
        pinpointPy.add_clue(FuncName, self.getFuncUniqueName())
        pinpointPy.add_clue(ServerType, MONGDB_EXE_QUERY)
        # pinpointPy.add_clues(PY_ARGS, )
        ###############################################################
        pinpointPy.add_clue("dst", self.dst)
        return args, kwargs

    def onEnd(self, ret):
        ###############################################################
        pinpointPy.add_clues(PY_RETURN, str(ret))
        ###############################################################
        super().onEnd(ret)
        return ret

    def onException(self, e):
        pinpointPy.add_clue('EXP', str(e))