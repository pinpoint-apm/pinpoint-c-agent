#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from pinpointPy.CommonPlugin import PinpointCommonPlugin


class Private(object):
    @PinpointCommonPlugin(__name__+'.Private.__private_func')
    def __private_func(self, arg):
        return "Private called by " + arg

    @PinpointCommonPlugin(__name__+'.Private.common_func')
    def common_func(self, name):
        r = self.__private_func(name)
        return r
