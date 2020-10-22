#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 8/20/20

from ..PinpointCommon import *
from ..PinpointDefine import *
import pinpointPy

class PyRedisPlugins(Candy):

    def __init__(self,class_name,module_name):
        super().__init__(class_name,module_name)
        self.dst = ''

    def onBefore(self,*args, **kwargs):
        args, kwargs = super().onBefore(*args, **kwargs)
        ###############################################################
        pinpointPy.add_clue(FuncName,self.getFuncUniqueName())
        pinpointPy.add_clue(ServerType, REDIS)
        arg = self.get_arg(*args, **kwargs)
        pinpointPy.add_clues(PY_ARGS, arg)
        ###############################################################
        if self.func_name == 'Redis':
            if 'host' in kwargs:
                self.dst = kwargs['host']+str(kwargs['port'])
            elif 'unix_socket_path' in kwargs:
                self.dst = kwargs['host'] + str(kwargs['port'])

        pinpointPy.add_clue("dst", self.dst)
        return args,kwargs

    def onEnd(self,ret):
        ###############################################################
        pinpointPy.add_clues(PY_RETURN,str(ret))
        ###############################################################
        super().onEnd(ret)
        return ret

    def onException(self, e):
        pinpointPy.add_clue('EXP',str(e))

    def get_arg(self, *args, **kwargs):
        args_tmp = {}
        j = 0

        for i in args:
            args_tmp["arg["+str(j)+"]"] = (str(i))
            j += 1
        # print(str(args_tmp))
        for k in kwargs:
            args_tmp[k] = kwargs[k]
        # print(str(args_tmp))
        return str(args_tmp)
