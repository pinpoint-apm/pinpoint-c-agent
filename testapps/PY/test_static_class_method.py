#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from pinpointPy.CommonPlugin import PinpointCommonPlugin


class Method(object):

    @staticmethod
    @PinpointCommonPlugin(__name__+'.Method.static_method')
    def static_method(a, b):
        return a + b

    @classmethod
    @PinpointCommonPlugin(__name__+'.Method.class_method')
    def class_method(cls, a, b):
        return cls.__name__ + a + b
