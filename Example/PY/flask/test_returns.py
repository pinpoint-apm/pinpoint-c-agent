#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from plugins.PinpointCommonPlugin import PinpointCommonPlugin


@PinpointCommonPlugin('', __name__)
def test_returns1(arg):
    return arg


@PinpointCommonPlugin('', __name__)
def test_returns2(a1, a2="Mer", *a3, **a4):
    a1 = a1 + " "
    a2 = a2 + "!"
    a3 = "[" + ",".join(a3) + "]"
    a4 = str(a4)
    return a1, a2, a3, a4
