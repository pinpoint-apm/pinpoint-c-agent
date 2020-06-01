#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 6/1/20
from CollectorAgent.GrpcClient import GrpcClient
from Proto.grpc.Service_pb2_grpc import StatStub
import threading

from Proto.grpc.Stat_pb2 import PStatMessage


class GrpcStat(GrpcClient):
    def __init__(self,address,meta):
        super().__init__(address, meta, -1)
        self.meta_stub = StatStub(self.channel)
        self.stat_thread = None
        self.exit_cv = threading.Condition()
        self.timeout = 5

    def channelSetReady(self):
        pass
    def channelSetIdle(self):
        pass
    def channelSetError(self):
        pass

    def stop(self):
        self.task_running =  True
        with self.exit_cv:
            self.exit_cv.notify_all()

        self.channel.close()

    def _generator_PstatMessage(self):
        while self.task_running:
            ps = PStatMessage()
            yield ps
            with self.exit_cv:
                if self.exit_cv.wait(self.timeout):
                    break

    def _stat_thread_main(self):
        self.task_running =  True
        self.meta_stub.SendAgentStat(self._generator_PstatMessage())

    def start(self):
        self.stat_thread = threading.Thread(target=self._stat_thread_main, args=())
        self.stat_thread.start()