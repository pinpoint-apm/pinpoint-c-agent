#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import os
import sys
cur_dir = os.path.dirname(os.path.realpath(__file__))
for dir in ['TCollectorAgent','PHPAgent','gen-py','Common','Trains','PinpointAgent']:
    sys.path.append(cur_dir+'/'+dir)

from TCollectorAgent import *
from PHPAgent import PHPAgent,PHPAgentConf
from Common import *
from CollectorAgentConf import CollectorAgentConf
from gevent.event import Event
import gevent,signal

class Server(object):
    def __init__(self):
        self.cac  = CollectorAgentConf(CAConfig)
        self.pac  = PHPAgentConf(CAConfig)
        self.thrift_agent = ThriftAgent(self.cac)
        self.php_agent    = PHPAgent(self.pac,self.thrift_agent.handlePHPAgentData)
        self.php_agent.registerPHPAgentHello(self.thrift_agent.tellWhoAMI)
        self.thrift_agent.startAll()
        self.php_agent.start()


    def run(self):
        _stop_event = Event()
        gevent.signal(signal.SIGQUIT, _stop_event.set)
        gevent.signal(signal.SIGTERM, _stop_event.set)
        gevent.signal(signal.SIGINT, _stop_event.set)
        _stop_event.wait()

        self.thrift_agent.stop()
        self.php_agent.stop()

        TCLogger.warning("collector agent exit with SIGNAL")


if __name__ == '__main__':
    server = Server()
    server.run()
