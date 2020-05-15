#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from pinpoint.plugins.PinpointCommonPlugin import PinpointCommonPlugin


@PinpointCommonPlugin('', __name__)
def in_lambda(b):
    return b * b


@PinpointCommonPlugin('', __name__)
def return_lambda(a):
    return lambda: in_lambda(a)

