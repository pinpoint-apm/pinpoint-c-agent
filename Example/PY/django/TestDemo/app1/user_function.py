#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from pinpoint.plugins.PinpointCommonPlugin import PinpointCommonPlugin


@PinpointCommonPlugin("", __name__)
def test_func3(arg1, arg2):
    return "this is test_func1: arg1=%s, arg2=%s"%(arg1, arg2)

class TestUserFunc2(object):

    def __init__(self, name, score):
        self.name = name
        self.score = score

    @PinpointCommonPlugin("TestUserFunc2", __name__)
    def test_func4(self):
        return "%s\'s score is : %s"%(self.name, self.score)
