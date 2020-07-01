#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 6/2/20
import os
import sys
import time
import unittest

from Common.Logger import logger_enable_console

proto_class = os.getcwd() + '/Proto/grpc'
sys.path.append(proto_class)


from CollectorAgent.GrpcStat import GrpcStat
from Proto.grpc.Service_pb2_grpc import StatServicer, add_StatServicer_to_server
import grpc
from concurrent import futures

class TestStat(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        class AgentStatServicer(StatServicer):
            def __init__(self):
                self.count = 0

            def SendAgentStat(self, request_iterator, context):
                for request in request_iterator:
                    # print(request)
                    self.count += 1

        cls.server = grpc.server(futures.ThreadPoolExecutor())
        cls.agentStat = AgentStatServicer()
        add_StatServicer_to_server(cls.agentStat, cls.server)
        cls.server.add_insecure_port('[::]:9992')
        cls.server.start()
        logger_enable_console()

    @classmethod
    def tearDownClass(cls) -> None:
        cls.server.stop(None)


    def __test(self):
        return (0,0)

    def test_stat(self):
        stat = GrpcStat('localhost:9992',
                        [('starttime', '1591001906164'), ('agentid', '345'), ('applicationname', '234324')],
                        self.__test, 1)
        # stat.task_running= True
        # print(next(stat._generator_PstatMessage()))
        stat.start()
        time.sleep(3)
        stat.stop()
        self.assertGreaterEqual(self.agentStat.count, 1)
