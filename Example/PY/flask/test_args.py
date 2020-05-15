#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from pinpoint.plugins.PinpointCommonPlugin import PinpointCommonPlugin


@PinpointCommonPlugin('', __name__)
def test_args1(arg):
    h = str(type(arg))[8:-2]
    return h


@PinpointCommonPlugin('', __name__)
def test_args2(a1, a2="Mer", *a3, **a4):
    h = "Positional arg:" + a1 + "<br>Default arg:" + a2 + "<br>Variadic arg:" + "<br>Variadic arg:[" + ','.join(a3) + "]<br>Keyword arg:" + str(a4)
    return h
