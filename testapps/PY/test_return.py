#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from pinpoint.common import PinpointCommonPlugin


def lazy_sum(*args):
    @PinpointCommonPlugin('', __name__)
    def sum():
        ax = 0
        for n in args:
            ax = ax + n
        return ax
    return sum
