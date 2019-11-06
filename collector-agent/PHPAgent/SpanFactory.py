#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 11/6/19

class SpanFactory(object):
    def __init__(self):
        self.sequenceid = 0

    def create_span(self,stackMap):
        raise NotImplemented

    def create_span_event(self,stackMap):
        raise NotImplemented

    def attach_span_event(self,span_event):
        raise NotImplemented

    def make_span(self,stackMap):
        self.sequenceid = 0
        span = self.create_span(stackMap)
        self.make_span_ev(span,stackMap)

        return span

    def make_span_ev(self,span,stackMap,index= 0 ):
        span_ev = self.create_span_event(stackMap)
        self.set_sequenceid(span_ev,self.sequenceid)
        self.sequenceid += 1
        self.attach_span_event(span_ev)

        if 'calls' in stackMap:
            for called in stackMap['calls']:
                self.make_span_ev(span,called, index + 1)

    def set_sequenceid(self,span_ev,id):
        raise NotImplemented