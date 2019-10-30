#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 10/21/19

class GTimer(object):
    @staticmethod
    def registerTimers(callback,interval,args=None):
        from gevent import get_hub
        loop = get_hub().loop
        time = loop.timer(0, interval)
        time.start(callback,args)