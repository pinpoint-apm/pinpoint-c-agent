# -*- coding: UTF-8 -*-
import grpc
import time
from unittest import TestCase
from Proto.grpc import Service_pb2_grpc
from Proto.grpc.Stat_pb2 import PAgentInfo


host='dev-pinpoint'
# agent_port=9991
# stat_port=9992
# span_port=9993


class TestGRPC(TestCase):
    def testStat(self):
        channel = grpc.insecure_channel('dev-pinpoint:9991')
        stub =Service_pb2_grpc.AgentStub(channel)

        response,call = stub.RequestAgentInfo.with_call(
            PAgentInfo(hostname='xxx', ip='ip', ports='456,546', pid=423, endTimestamp=-1,
                       serviceType=1500),
            metadata=(('agentid', 'test-id'),
                      ('applicationname', 'test-name'),
                      ('starttime', str(int(time.time() * 1000))))
        )

        # agent_info = PAgentInfo(hostname='xxx',ip='ip',ports='456,546',pid=423,endTimestamp=-1,
        #                         serviceType=1500)

        #
        # result = stub.RequestAgentInfo(agent_info,metedata={('agentid','test-id'),
        #                                                     ('applicationname','test-name'),
        #                                                     ('starttime', int(time.time()*1000))})
        # print(result)

        for key, value in call.trailing_metadata():
            print('%s %s'%(key,value))