#!/usr/bin/env python
# -*- coding: UTF-8 -*-


import abc
from pinpointPy.CommonPlugin import PinpointCommonPlugin


class AllFle(metaclass=abc.ABCMeta):
    all_type='file'
    @PinpointCommonPlugin(__name__+'.AllFle.read')
    @abc.abstractmethod
    def read(self):
        pass


class Txt(AllFle):
    @PinpointCommonPlugin(__name__+'.Txt.read')
    def read(self):
        return "Reading txt!"
