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
# -*- coding: UTF-8 -*-


E_CONNECTED = 0
E_CONNETING = 1
E_WRITING = 2
E_READ = 4
E_CLOSED  = 8

from gevent import get_hub

class TrainLayer(object):
    # remote = (host,port)
    #
    def __init__(self,remote,msg_cb,auto_recover=True,timeout= 2):
        self.timeout     = timeout
        self.remote      = remote
        self.in_msg_cb     = msg_cb
        self.state = E_CLOSED

    def start(self):
        pass

    def stop(self):
        pass

    @staticmethod
    def registerTimers(callback,interval,args=None):
        loop = get_hub().loop
        time = loop.timer(0, interval)
        time.start(callback,args)


if __name__ == '__main__':
    # print(gsocket.getaddrinfo("www.google.jp", 80))
    pass
