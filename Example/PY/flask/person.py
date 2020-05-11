#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from plugins.PinpointCommonPlugin import PinpointCommonPlugin


class Person(object):
    @PinpointCommonPlugin("Person", __name__)
    def eat(self):
        return "father eating!"
