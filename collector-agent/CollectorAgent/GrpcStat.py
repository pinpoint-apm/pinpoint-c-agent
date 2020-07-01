#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 6/1/20
import threading
import time

import psutil

from CollectorAgent.GrpcClient import GrpcClient
from Common.Logger import TCLogger
from PinpointAgent.Type import STAT_INTERVAL
from Proto.grpc.Service_pb2_grpc import StatStub
from Proto.grpc.Stat_pb2 import PStatMessage, PAgentStat, PResponseTime


class GrpcStat(GrpcClient):
    def __init__(self, address, meta, get_inter_stat_cb, timeout=5):
        super().__init__(address, meta, -1)
        self.meta_stub = StatStub(self.channel)
        self.stat_thread = None
        self.exit_cv = threading.Condition()
        self.timeout = timeout
        assert get_inter_stat_cb
        self.get_inter_stat_cb = get_inter_stat_cb
        self.task_running = False
    def channelSetReady(self):
        pass
    def channelSetIdle(self):
        pass
    def channelSetError(self):
        pass

    def stop(self):
        self.task_running = False
        with self.exit_cv:
            self.exit_cv.notify_all()
        self.channel.close()


    def _generAgentStat(self):
        max,avg = self.get_inter_stat_cb()
        responseTime = PResponseTime(max=max,avg=avg)
        stat = PAgentStat(responseTime=responseTime)
        stat.timestamp = int(round(time.time()*1000))
        stat.collectInterval = STAT_INTERVAL
        stat.cpuLoad.systemCpuLoad =psutil.cpu_percent()*0.01
        return stat

    def _generator_PstatMessage(self):
        try:
            while self.task_running:
                ps = PStatMessage(agentStat=self._generAgentStat())
                TCLogger.debug(ps)
                yield ps
                time.sleep(STAT_INTERVAL)
        except Exception as e:
            TCLogger.warning("_generator_PstatMessage catch %s", e)

    def _stat_thread_main(self):
        self.task_running = True
        while self.task_running:
            try:
                self.meta_stub.SendAgentStat(self._generator_PstatMessage())
            except Exception as e:
                TCLogger.warning("SendAgentStat met:%s",e)
            with self.exit_cv:
                if self.exit_cv.wait(self.timeout):
                    break


    def start(self):
        self.stat_thread = threading.Thread(target=self._stat_thread_main, args=())
        self.stat_thread.start()

