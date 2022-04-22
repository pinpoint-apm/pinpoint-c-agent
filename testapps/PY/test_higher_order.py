#!/usr/bin/env python
# -*- coding: UTF-8 -*-
from pinpointPy.CommonPlugin import PinpointCommonPlugin


@PinpointCommonPlugin( __name__+".f")
def f(x:int):
    return x * x
