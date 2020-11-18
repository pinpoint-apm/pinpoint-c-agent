#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from person import Person
from pinpoint.common import PinpointCommonPlugin


class Doctor(Person):

    @PinpointCommonPlugin("Doctor"+ __name__)
    def other(self):
        return "Doctor not eating!"
