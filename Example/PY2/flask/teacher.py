#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from person import Person
from pinpoint.plugins.PinpointCommonPlugin import PinpointCommonPlugin


class Teacher(Person):

    @PinpointCommonPlugin("Teacher", __name__)
    def eat(self):
        return super(Teacher,self).eat()+"Teacher eating too!"
