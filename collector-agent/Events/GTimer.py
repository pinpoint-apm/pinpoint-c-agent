#!/usr/bin/env python
# -*- coding: UTF-8 -*-# Created by eeliu at 10/21/19

# ------------------------------------------------------------------------------
#  Copyright  2020. NAVER Corp.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
# ------------------------------------------------------------------------------
from gevent import get_hub
class GTimer(object):
    def __init__(self,only_once = True):
        self.loop = get_hub().loop
        self.timer = None
        self.callback = None
        self.only_once = only_once
        self.started = False

    def _timerCallback(self,*args):
        self.callback(*args)
        if self.only_once :
            self.timer.stop()
            self.started = False

    def start(self, callback, interval, *args):
        if self.started:
            return

        self.started = True
        self.timer = self.loop.timer(interval,interval)
        self.callback = callback
        self.timer.start(self._timerCallback,*args)

    def stop(self):
        if not self.only_once and self.timer:
            self.timer.stop()
            self.started = False

