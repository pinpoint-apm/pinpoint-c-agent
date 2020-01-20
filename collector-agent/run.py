


#!/usr/bin/env python
# -*- coding: UTF-8 -*-
import os,sys
# !/usr/bin/env python
# -*- coding: UTF-8 -*-
import os
import sys

proto_class = os.getcwd()+'/Proto/grpc'
sys.path.append(proto_class)

import gevent

from CollectorAgent.CollectorAgentConf import CollectorAgentConf
from Common.Config import CAConfig
from Common.Logger import TCLogger
from PHPAgent import PHPAgentConf, FrontAgent
from PinpointAgent.AppManagement import AppManagement


class Server(object):
    def __init__(self):
        self.collector_conf = CollectorAgentConf(CAConfig)
        self.app_management = AppManagement(self.collector_conf)
        self.pac = PHPAgentConf(CAConfig)
        self.php_agent = FrontAgent(self.pac, self.app_management.handle_front_agent_data)
        self.php_agent.registerPHPAgentHello(self.app_management.tell_whoami)
        self.php_agent.start()

    def run(self):
        while True:
            gevent.sleep(10)
        # break by a signal
        self.app_management.stop_all()
        self.php_agent.stop()
        TCLogger.warning("collector agent exit with SIGNAL")

if __name__ == '__main__':
    server = Server()
    server.run()
