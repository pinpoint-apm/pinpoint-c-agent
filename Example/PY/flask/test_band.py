#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from pinpoint.plugins.PinpointCommonPlugin import PinpointCommonPlugin


@PinpointCommonPlugin('', __name__)
def band_func(self, x):
    self.x = x
    return self.x * self.x


class BandClass(object):
    pass
