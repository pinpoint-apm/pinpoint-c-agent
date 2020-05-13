#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from TestDemo.person import Person
from plugins.PinpointCommonPlugin import PinpointCommonPlugin


class Doctor(Person):

    @PinpointCommonPlugin("Doctor", __name__)
    def other(self):
        return "Doctor not eating!"
