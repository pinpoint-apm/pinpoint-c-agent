#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from pinpointPy.CommonPlugin import PinpointCommonPlugin


@PinpointCommonPlugin(__name__+".in_lambda")
def in_lambda(b):
    return b * b


@PinpointCommonPlugin( __name__+".return_lambda")
def return_lambda(a):
    return lambda: in_lambda(a)

