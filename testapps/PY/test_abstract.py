#!/usr/bin/env python
# -*- coding: UTF-8 -*-


import abc
from pinpoint.common import PinpointCommonPlugin


class AllFle(metaclass=abc.ABCMeta):
    all_type='file'
    @PinpointCommonPlugin('AllFle'+ __name__)
    @abc.abstractmethod
    def read(self):
        pass


class Txt(AllFle):
    @PinpointCommonPlugin('Txt'+__name__)
    def read(self):
        return "Reading txt!"
