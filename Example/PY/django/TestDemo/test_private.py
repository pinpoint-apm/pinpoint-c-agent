#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from plugins.PinpointCommonPlugin import PinpointCommonPlugin


class Private(object):
    @PinpointCommonPlugin('Private', __name__)
    def __private_func(self, arg):
        return "Private called by " + arg

    @PinpointCommonPlugin('Private', __name__)
    def common_func(self, name):
        r = self.__private_func(name)
        return r
