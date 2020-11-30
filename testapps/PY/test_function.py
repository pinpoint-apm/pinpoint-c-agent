#!/usr/bin/env python
# -*- coding: UTF-8 -*-
from pinpoint.common import PinpointCommonPlugin


@PinpointCommonPlugin(__name__)
def test_func1(arg1, arg2):
    return "this is test_func1: arg1=%s, arg2=%s"%(arg1, arg2)


class TestUserFunc1(object):

    def __init__(self, name, score):
        self.name = name
        self.score = score

    @PinpointCommonPlugin(__name__)
    def test_func2(self):
        return "%s\'s score is : %s"%(self.name, self.score)



