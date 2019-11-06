#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 10/18/19\
import time
from CollectorAgent.CollectorAgentConf import CollectorAgentConf
from CollectorAgent.GrpcAgentImplement import GrpcAgentImplement
from Common.Config import CAConfig


class FakeManager(object):
    pass

if __name__ == '__main__':
    collector_conf = CollectorAgentConf(CAConfig)
    agent = GrpcAgentImplement(collector_conf,'test-id','test-id-15')
    agent.start()
    time.sleep(3)
