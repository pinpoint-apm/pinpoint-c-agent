#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 6/2/20
import time,os,sys
import unittest
proto_class = os.getcwd()+'/../Proto/grpc'
sys.path.append(proto_class)


from CollectorAgent.GrpcStat import GrpcStat


class TestStat(unittest.TestCase):

    def __test(self):
        return (0,0)

    def test_stat(self):

        stat = GrpcStat('dev-pinpoint:9992',[('starttime','1591001906164'),('agentid','345'),('applicationname','234324')],self.__test)
        # stat.task_running= True
        # print(next(stat._generator_PstatMessage()))
        stat.start()
        time.sleep(10)
        stat.stop()