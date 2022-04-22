#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from person import Person
from pinpointPy.CommonPlugin import PinpointCommonPlugin


class Student(Person):
    @PinpointCommonPlugin(__name__+".Student.eat")
    def eat(self):
        return "Student eating"
