#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 10/18/19\
from CollectorAgent import CollectorAgentConf
from CollectorAgent.GrpcAgentImplement import GrpcAgentImplement
import time
from Common.Config import CAConfig


class FakeManager(object):
    pass

if __name__ == '__main__':
    collector_conf = CollectorAgentConf(CAConfig)
    manager = FakeManager()
    agent = GrpcAgentImplement(manager,collector_conf,'test-id','test-id-15')
    agent.start()
    # time.sleep(3)
