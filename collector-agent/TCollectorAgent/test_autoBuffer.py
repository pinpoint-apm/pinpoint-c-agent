#-------------------------------------------------------------------------------
# Copyright 2019 NAVER Corp
# 
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License.  You may obtain a copy
# of the License at
# 
#   http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations under
# the License.
#-------------------------------------------------------------------------------
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
