#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 10/21/19


# -*- coding: UTF-8 -*-
import time
import logging
from unittest import TestCase
from CollectorAgent.AgentClient import AgentClient
from Common.Logger import TCLogger

class TestGRPCRoutine(TestCase):
    def setUp(self) -> None:
        # import sys, os
        # sys.path.append(os.path.abspath('..'))
        # pass
        import sys
        TCLogger= logging.getLogger()
        TCLogger.level=logging.DEBUG
        console_handler=logging.StreamHandler(sys.stdout)
        TCLogger.addHandler(console_handler)
    def test_agentinfo(self):
        starttime = str(int(time.time() * 1000))
        agent_meta = [('agentid', 'test-id'),
                      ('applicationname', 'test-name'),
                      ('starttime', starttime)]
        agent = AgentClient('dev-1230','10.10.12.23','2345',4569,'dev-pinpoint:9991',agent_meta)
        time.sleep(10)
        agent.stop()