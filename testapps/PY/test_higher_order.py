#!/usr/bin/env python
# -*- coding: UTF-8 -*-
from pinpoint.common import PinpointCommonPlugin


@PinpointCommonPlugin( __name__)
def f(x:int):
    return x * x
