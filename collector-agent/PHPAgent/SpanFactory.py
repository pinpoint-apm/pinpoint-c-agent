#!/usr/bin/env python
# -*- coding: UTF-8 -*-

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

# Created by eeliu at 11/6/19


class SpanFactory(object):
    def __init__(self,agent):
        self.sequenceid = 0
        self.agent = agent

    def createSpan(self, stackMap):
        raise NotImplemented

    def createSpanEvent(self, stackMap):
        raise NotImplemented

    def attachSpanEvent(self, span, span_event):
        raise NotImplemented

    def makeSpan(self, stackMap):
        self.sequenceid = 0
        span = self.createSpan(stackMap)
        if 'calls' in stackMap:
            for called in stackMap['calls']:
                self.makeSpanEv(span, called)
        return span

    def makeSpanEv(self, span, stackMap, depth=1):
        span_ev = self.createSpanEvent(stackMap)
        self.setSequenceid(span_ev, self.sequenceid)
        self.sequenceid += 1
        self.setDepth(span_ev, depth)
        self.attachSpanEvent(span, span_ev)
        if 'calls' in stackMap:
            for called in stackMap['calls']:
                self.makeSpanEv(span, called, depth + 1)

    def setSequenceid(self, span_ev, id):
        raise NotImplemented

    def setDepth(self, span_ev, depth):
        raise NotImplemented