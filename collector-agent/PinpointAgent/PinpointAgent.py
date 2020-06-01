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
from PinpointAgent.Type import PHP
import time

class PinpointAgent(object):

    class ReqStatCount(object):
        def __init__(self):
            self.req_stat_count = [0, 0, 0, 0]
            self.last_time =int(time.time())

        def updateStat(self,req_time):
            req_time = int(req_time/1000)
            last_time = int(time.time())
            if last_time != self.last_time:
                self.req_stat_count = [0, 0, 0, 0]
            self.req_stat_count[self._reqLevel(req_time)] += 1
            self.last_time = last_time
        def _reqLevel(self,exp):

       #
       #      1s: faster
       #      3s: normal
       #      5s: Slow
       #      >5s: very slow
       #
            if exp <= 1:
                return 0
            elif exp <=3:
                return 1
            elif exp <=5:
                return 2
            else:
                return 3

        def getReqStat(self):
            last_time = int(time.time())
            if last_time < self.last_time +2:
                return self.req_stat_count
            else:
                return (0,0,0,0)


    def __init__(self,app_id,app_name,serviceType=PHP):
        self.app_id =app_id
        self.app_name = app_name
        self.service_type = serviceType
        self.req_stat = PinpointAgent.ReqStatCount();
    def start(self):
        raise NotImplementedError()

    def sendSpan(self,stack,body):
        exp =int(stack['E'])
        self.req_stat.updateStat(exp)

    def stop(self):
        raise NotImplementedError()

    def getReqStat(self):
        return self.req_stat.getReqStat()
