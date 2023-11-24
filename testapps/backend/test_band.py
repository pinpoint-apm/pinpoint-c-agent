#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from pinpointPy.CommonPlugin import PinpointCommonPlugin


@PinpointCommonPlugin(__name__+".band_func")
def band_func(self, x):
    self.x = x
    return self.x * self.x


class BandClass(object):
    pass
