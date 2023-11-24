#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from pinpointPy.CommonPlugin import PinpointCommonPlugin


class Special(object):
    @PinpointCommonPlugin(__name__+'.Special.__init__')
    def __init__(self, name, age):
        self.name = name
        self.age = age

    @PinpointCommonPlugin(__name__+'.Special.common_func')
    def common_func(self):
        return "%s is %d years old." % (self.name, self.age)
