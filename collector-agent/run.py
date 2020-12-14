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

# !/usr/bin/env python
# -*- coding: UTF-8 -*-
import os
import sys

proto_class = os.getcwd()+'/Proto/grpc'
sys.path.append(proto_class)

from Common import TCLogger
from PinpointAgent import start_pinpoint_agent,stop_pinpoint_agent

class Server(object):
    def __init__(self):
        start_pinpoint_agent()

    def run(self):
        while True:
            try:
                import gevent
                gevent.sleep(10)
            except:
                break
        stop_pinpoint_agent()
        TCLogger.warning("collector agent exit with SIGNAL")

if __name__ == '__main__':
    server = Server()
    server.run()
