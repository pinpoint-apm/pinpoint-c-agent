#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from pinpointPy.CommonPlugin import PinpointCommonPlugin


class Animal(object):
    @PinpointCommonPlugin(__name__+'.Animal.animal_func')
    def animal_func(self):
        return " and I am an animal"


class ThinkMixIn(object):
    @PinpointCommonPlugin(__name__+'.ThinkMixIn.think_func')
    def think_func(self):
        return " and I can think"


class Human(Animal, ThinkMixIn):
    @PinpointCommonPlugin( __name__+'.Human.human_func')
    def human_func(self):
        return "I am a human"