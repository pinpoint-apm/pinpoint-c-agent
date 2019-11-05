#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 10/14/19
import grpc
import time

from gevent import monkey
monkey.patch_all()
import grpc.experimental.gevent as grpc_gevent
grpc_gevent.init_gevent()

from PinpointAgent.Type import PHP
from Proto.grpc import Service_pb2_grpc
from Proto.grpc.Stat_pb2 import PAgentInfo, PPing


from Span_pb2 import PSqlMetaData, PApiMetaData, PSpanMessage, PSpan, PTransactionId


def agentinfo():
    channel = grpc.insecure_channel('dev-pinpoint:9991')
    stub = Service_pb2_grpc.AgentStub(channel)
    starttime = str(int(time.time() * 1000))
    agent_meta =[('agentid', 'test-id'),
                  ('applicationname', 'test-name'),
                  ('starttime',starttime)]

    response, call = stub.RequestAgentInfo.with_call(
        PAgentInfo(hostname='xxx', ip='ip', ports='456', pid=423, endTimestamp=-1,
                   serviceType=PHP),
        metadata=agent_meta
    )

    print(response)
    print(call.trailing_metadata())

    # def ping_reqeust():
    #     for i in range(10):
    #         ping = PPing()
    #         time.sleep(1)
    #         print('-')
    #         yield ping
    #
    # ping_meta = agent_meta[:]
    # ping_meta.append(('socketid', '55'))
    # ping_iter = stub.PingSession(ping_reqeust(),metadata=ping_meta)
    #
    # for ping in ping_iter:
    #     print(ping)


    ## send metadata
    # channel = grpc.insecure_channel('dev-pinpoint:9991')
    #
    # meta_stub = Service_pb2_grpc.MetadataStub(channel)
    #
    # result = meta_stub.RequestApiMetaData(PApiMetaData(apiId=1,apiInfo='name',line=10,type=0),metadata=agent_meta)
    # print(result)
    #
    # ## snd span
    channel = grpc.insecure_channel('dev-pinpoint:9993')
    span_stub = Service_pb2_grpc.SpanStub(channel)
    #
    def request_span():
        for i in range(5):
            tid = PTransactionId(agentId='test-id',agentStartTime=int(time.time()),sequence=i)
            span =PSpan(version = 1,
                        transactionId=tid,
                        startTime= int(time.time()),
                        elapsed=10,
                        apiId=1,
                        serviceType=1500,
                        applicationServiceType=1500)
            meg= PSpanMessage(span=span)
            yield meg
    print("send span")

    def resp(self,*args):
        print(args)

    response = span_stub.SendSpan.future(request_span(),metadata=agent_meta)
    response.add_done_callback(resp)

    time.sleep(3)

    # print(call.trailing_metadata())




    # for r in results:
    #     print(r)


    # while True:
    #     response, call = stub.PingSession(PPing(), metadata=agent_meta)
    #     print(response)
    #     print(call)
    #     time.sleep(1)



if __name__ == '__main__':
    agentinfo()