#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 6/2/20

import  time

from PinpointAgent.Type import STAT_INTERVAL


class ReqStat(object):

    class IntervalStat(object):
        def __init__(self):
            self.max = 0
            self.total = 0
            self.times = 0
            self.last_time = 0
        def reset(self):
            self.last_time = self.max = self.total = self.times = 0

        def update(self,req_time):
            last_time = int(time.time())
            if last_time >= self.last_time + STAT_INTERVAL+1:
                self.reset()
            self.times +=1
            self.max = req_time if req_time > self.max else self.max
            self.total += req_time
            self.last_time = last_time

        def getInterStat(self):
            last_time = int(time.time())
            if last_time >= self.last_time + STAT_INTERVAL+1:
                return (0,0)
            else:
                return (self.max,int(self.total/self.times))

    def __init__(self):
        self.req_stat_count = [0, 0, 0, 0]
        self.last_time = int(time.time())
        self.inter_stat = ReqStat.IntervalStat()

    def updateStat(self, req_time):
        # update interval stat
        self.inter_stat.update(req_time)

        # update request stat
        req_time = int(req_time / 1000)
        last_time = int(time.time())
        if last_time != self.last_time:
            self.req_stat_count = [0, 0, 0, 0]
        self.req_stat_count[self._reqLevel(req_time)] += 1
        self.last_time = last_time




    def _reqLevel(self, exp):
        #
        #      1s: faster
        #      3s: normal
        #      5s: Slow
        #      >5s: very slow
        #
        if exp <= 1:
            return 0
        elif exp <= 3:
            return 1
        elif exp <= 5:
            return 2
        else:
            return 3


    def getReqStat(self):
        last_time = int(time.time())
        if last_time < self.last_time + 2:
            return self.req_stat_count
        else:
            return (0, 0, 0, 0)


    def getInervalStat(self):
        return self.inter_stat.getInterStat()