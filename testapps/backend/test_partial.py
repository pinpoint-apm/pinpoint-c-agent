#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from pinpointPy.CommonPlugin import PinpointCommonPlugin


@PinpointCommonPlugin( __name__+".func1")
def func1(a, b="Wool"):
    return a + b
