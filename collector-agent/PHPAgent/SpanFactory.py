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

    def create_span(self,stackMap):
        raise NotImplemented

    def create_span_event(self,stackMap):
        raise NotImplemented

    def attach_span_event(self,span,span_event):
        raise NotImplemented

    def make_span(self,stackMap):
        self.sequenceid = 0
        span = self.create_span(stackMap)
        if 'calls' in stackMap:
            for called in stackMap['calls']:
                self.make_span_ev(span,called)
        # TCLogger.debug("send span:%s",span)
        return span

    def make_span_ev(self, span, stackMap, depth=1):
        span_ev = self.create_span_event(stackMap)
        self.set_sequenceid(span_ev,self.sequenceid)
        self.sequenceid += 1
        self.set_depth(span_ev,depth)
        self.attach_span_event(span,span_ev)
        # TCLogger.debug(span_ev)
        if 'calls' in stackMap:
            for called in stackMap['calls']:
                self.make_span_ev(span, called, depth + 1)

    def set_sequenceid(self,span_ev,id):
        raise NotImplemented

    def set_depth(self,span_ev,depth):
        raise NotImplemented