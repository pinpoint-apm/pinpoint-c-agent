#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from plugins.PinpointCommonPlugin import PinpointCommonPlugin


class Special(object):
    @PinpointCommonPlugin('Special', __name__)
    def __init__(self, name, age):
        self.name = name
        self.age = age

    @PinpointCommonPlugin('Special', __name__)
    def common_func(self):
        return "%s is %d years old." % (self.name, self.age)
