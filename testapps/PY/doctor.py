#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from person import Person
from pinpointPy.CommonPlugin import PinpointCommonPlugin


class Doctor(Person):

    @PinpointCommonPlugin(__name__+".Doctor.other")
    def other(self):
        return "Doctor not eating!"
