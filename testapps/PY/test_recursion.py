#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from pinpointPy.CommonPlugin import PinpointCommonPlugin


@PinpointCommonPlugin(__name__)
def fact(n):
    if n==1:
        return 1
    return n * fact(n - 1)