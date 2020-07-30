#!/usr/bin/env python
# -*- coding: UTF-8 -*-
from pinpoint.plugins.PinpointCommonPlugin import PinpointCommonPlugin


@PinpointCommonPlugin('', __name__)
def f(x):
    return x * x
