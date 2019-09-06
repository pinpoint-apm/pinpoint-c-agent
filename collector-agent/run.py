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

from PHPAgent import *
from Common import *
from CollectorAgent.CollectorAgentConf import CollectorAgentConf
from gevent.event import Event
from PinpointAgent.AppManagement import  AppManagement
import gevent,signal


class Server(object):
    def __init__(self):
        self.collector_conf = CollectorAgentConf(CAConfig)
        self.pac = PHPAgentConf(CAConfig)
        self.app_management = AppManagement(self.collector_conf)
        self.php_agent = FrontAgent(self.pac, self.app_management.handle_front_agent_data)
        self.php_agent.registerPHPAgentHello(self.app_management.tell_whoami)
        self.php_agent.start()

    def run(self):
        _stop_event = Event()
        gevent.signal(signal.SIGQUIT, _stop_event.set)
        gevent.signal(signal.SIGTERM, _stop_event.set)
        gevent.signal(signal.SIGINT, _stop_event.set)
        _stop_event.wait()
        self.app_management.stop_all()
        self.php_agent.stop()
        TCLogger.warning("collector agent exit with SIGNAL")

if __name__ == '__main__':
    server = Server()
    server.run()
