#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from TestDemo.person import Person
from plugins.PinpointCommonPlugin import PinpointCommonPlugin


class Student(Person):
    @PinpointCommonPlugin("Student", __name__)
    def eat(self):
        return "Student eating"
