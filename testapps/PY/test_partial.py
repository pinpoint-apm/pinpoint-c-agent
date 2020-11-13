#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from pinpoint.common import PinpointCommonPlugin


@PinpointCommonPlugin( __name__)
def func1(a, b="Wool"):
    return a + b
