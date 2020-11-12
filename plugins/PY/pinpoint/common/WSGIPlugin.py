#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 11/9/20

from .Common import Candy
from .Utils import *

class WSGIPlugin(Candy):
    def __init__(self, name):
        super().__init__(name)

    def onBefore(self, *args, **kwargs):
        super().onBefore(*args, **kwargs)
        environ = args[0]
        startPinpointByEnviron(environ)

        return (args, kwargs)

    def onEnd(self, ret):
        endPinpointByEnviron(ret)
        super().onEnd(ret)
        return ret

    def onException(self, e):
        pinpointPy.mark_as_error(e,"",0)