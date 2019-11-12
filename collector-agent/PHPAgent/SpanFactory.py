#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 11/6/19
from Common.Logger import TCLogger
from Span_pb2 import PSpanMessage


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
        TCLogger.debug("send span:%s",span)
        return span

    def make_span_ev(self, span, stackMap, depth=1):
        span_ev = self.create_span_event(stackMap)
        self.set_sequenceid(span_ev,self.sequenceid)
        self.sequenceid += 1
        self.set_depth(span_ev,depth)
        self.attach_span_event(span,span_ev)
        TCLogger.debug(span_ev)
        if 'calls' in stackMap:
            for called in stackMap['calls']:
                self.make_span_ev(span, called, depth + 1)

    def set_sequenceid(self,span_ev,id):
        raise NotImplemented

    def set_depth(self,span_ev,depth):
        raise NotImplemented