#!/usr/bin/env python
from unittest import TestCase

from Protocol import AutoBuffer


# -*- coding: UTF-8 -*-
class TestAutoBuffer(TestCase):
    def test_putSVar(self):
        ab =  AutoBuffer()
        ab.putSVar(32)


    def test_putVar32(self):
        self.fail()

    def test_putPrefixedString(self):
        self.fail()

    def test_putVar64(self):
        self.fail()
