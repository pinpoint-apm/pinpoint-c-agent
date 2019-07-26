#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import socket
from Config import CAConfig

class AgentHost(object):
    def __init__(self):
        self.hostname = socket.gethostname()
        self.ip   = socket.gethostbyname(self.hostname)
        self.port = CAConfig.get('Common','Web_Port')
    def __str__(self):
        return "host:%s ip:%s"%(self.hostname,self.ip)

if __name__ == '__main__':
    agent = AgentHost()
    print(agent)