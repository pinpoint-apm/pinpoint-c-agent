#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from person import Person
from pinpoint.common import PinpointCommonPlugin


class Student(Person):
    @PinpointCommonPlugin("Student"+ __name__)
    def eat(self):
        return "Student eating"
