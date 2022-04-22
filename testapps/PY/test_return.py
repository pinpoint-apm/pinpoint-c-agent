#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from pinpointPy.CommonPlugin import PinpointCommonPlugin


def lazy_sum(*args):
    @PinpointCommonPlugin(__name__+".sum")
    def sum():
        ax = 0
        for n in args:
            ax = ax + n
        return ax
    return sum
