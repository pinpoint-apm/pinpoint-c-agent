#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from pinpointPy.CommonPlugin import PinpointCommonPlugin


class Person(object):
    @PinpointCommonPlugin(__name__+".Person.eat")
    def eat(self):
        return "father eating!"
