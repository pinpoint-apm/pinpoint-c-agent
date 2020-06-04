#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 10/21/19


# -*- coding: UTF-8 -*-
import os
import sys
import time

proto_class = os.getcwd() + '/Proto/grpc'
sys.path.append(proto_class)

from concurrent import futures
from queue import Queue
import grpc
from unittest import TestCase
from CollectorAgent.GrpcMeta import GrpcMeta
from CollectorAgent.GrpcSpan import GrpcSpan
from Common.Logger import logger_enable_console
from Proto.grpc.Service_pb2_grpc import SpanServicer, add_SpanServicer_to_server, MetadataServicer, \
    add_MetadataServicer_to_server
from Proto.grpc.Span_pb2 import PTransactionId, PSpan, PSpanMessage, PResult


class TestGRPCRoutine(TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        class SendSpanServicer(SpanServicer):
            def __init__(self):
                self.count = 0

            def SendSpan(self, request_iterator, context):
                for request in request_iterator:
                    print(request)
                    self.count += 1

        class MetaServicer(MetadataServicer):
            def __init__(self):
                self.count = 0

            def _print_and_count(self, request):
                print(request)
                self.count += 1
                return PResult(success=True, message='success')

            def RequestSqlMetaData(self, request, context):
                return self._print_and_count(request)

            def RequestApiMetaData(self, request, context):
                return self._print_and_count(request)

            def RequestStringMetaData(self, request, context):
                return self._print_and_count(request)

        cls.server = grpc.server(futures.ThreadPoolExecutor())
        cls.spanStat = SendSpanServicer()
        cls.metaStat = MetaServicer()
        add_SpanServicer_to_server(cls.spanStat, cls.server)
        add_MetadataServicer_to_server(cls.metaStat, cls.server)

        cls.server.add_insecure_port('[::]:9993')
        cls.server.start()
        logger_enable_console()

        cls.agent_meta = [('agentid', 'test-id'),
                          ('applicationname', 'test-name'),
                          ('starttime', str(int(time.time() * 1000)))]

    @classmethod
    def tearDownClass(cls) -> None:
        cls.server.stop(None)

    SEQ = 0

    def _generate_span(self):

        tid = PTransactionId(agentId='test-id',agentStartTime=int(time.time()),sequence=TestGRPCRoutine.SEQ)
        span =PSpan(version = 1,
                    transactionId=tid,
                    startTime= int(time.time()),
                    elapsed=10,
                    apiId=1,
                    serviceType=1500,
                    applicationServiceType=1500)
        TestGRPCRoutine.SEQ+= 1
        msg= PSpanMessage(span=span)
        print("generator a span")
        return msg

    def test_sendspan(self):

        queue = Queue()
        spanclient = GrpcSpan('localhost:9993', self.agent_meta, queue)
        spanclient.start()
        i = 100
        while i > 0:
            queue.put(self._generate_span())
            i -= 1
        time.sleep(1)
        spanclient.stop()
        self.assertGreaterEqual(self.spanStat.count, 100)

    def test_metaData(self):
        apis=(['aa',10,0],['ada',12,0],['aa',31,2],['aaf',11,3])
        strings = ('aaaaaaaaaaaa','bbbbbbbbbbb','cccccccc')
        sqls=('ssss','bbbbb','ssssssss')
        meta_client = GrpcMeta('localhost:9993', self.agent_meta)
        id = 0
        for api in apis:
            id = meta_client.updateApiMeta(*api)
        self.assertEqual(id, 4)
        for string in strings:
            id = meta_client.updateStringMeta(string)
        self.assertEqual(id, 7)
        for sql in sqls:
            id = meta_client.updateSqlMeta(sql)
        self.assertEqual(id, 10)

        id = meta_client.updateSqlMeta('ssss')
        self.assertEqual(id, 8)

        id = meta_client.updateStringMeta('bbbbbbbbbbb')
        self.assertEqual(id, 6)
        time.sleep(1)
        self.assertGreaterEqual(self.metaStat.count, 10)
