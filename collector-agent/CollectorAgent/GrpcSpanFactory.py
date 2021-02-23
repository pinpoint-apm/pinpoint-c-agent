#!/usr/bin/env python
# -*- coding: UTF-8 -*-

# ------------------------------------------------------------------------------
#  Copyright  2020. NAVER Corp.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
# ------------------------------------------------------------------------------

# Created by eeliu at 11/7/19


from google.protobuf.wrappers_pb2 import StringValue

from Annotation_pb2 import PAnnotation, PAnnotationValue, PLongIntIntByteByteStringValue
from CollectorAgent.TCGenerator import ThriftProtocolUtil
from Common.Logger import TCLogger
from PHPAgent.SpanFactory import SpanFactory
from PinpointAgent.Type import *
from Proto.grpc.Annotation_pb2 import PIntStringStringValue
from Span_pb2 import PSpan, PSpanEvent


class GrpcSpanFactory(SpanFactory):
    def createSpan(self, stackMap):
        tSpan = PSpan()
        tSpan.apiId = self.agent.updateApiMeta(stackMap['name'], API_WEB_REQUEST)
        tSpan.version = 1
        if 'stp' in stackMap:
            tSpan.serviceType = int(stackMap['stp'])
            tSpan.applicationServiceType = int(stackMap['FT'])
        else:
            tSpan.serviceType = PHP
            tSpan.applicationServiceType = PHP

        if 'psid' in stackMap:
            tSpan.parentSpanId = int(stackMap['psid'])
        else:
            tSpan.parentSpanId = -1

        if 'tid' in stackMap:
            agentId, startTime, id = stackMap['tid'].split('^')
            tSpan.transactionId.agentId = agentId
            tSpan.transactionId.agentStartTime = int(startTime)
            tSpan.transactionId.sequence = int(id)

        if 'sid' in stackMap:
            tSpan.spanId = int(stackMap['sid'])

        if 'S' in stackMap:
            tSpan.startTime = stackMap['S']

        if 'E' in stackMap:
            tSpan.elapsed = stackMap['E']

        accept = tSpan.acceptEvent

        if 'uri' in stackMap:
            accept.rpc = stackMap['uri']
        if 'server' in stackMap:
            accept.endPoint = stackMap['server']
        if 'client' in stackMap:
            accept.remoteAddr = stackMap['client']

        parent_info = accept.parentInfo

        if 'pname' in stackMap:
            parent_info.parentApplicationName = stackMap['pname']

        if 'ptype' in stackMap:
            parent_info.parentApplicationType = int(stackMap['ptype'])

        if 'Ah' in stackMap:
            parent_info.acceptorHost = stackMap['Ah']

        if 'ERR' in stackMap:
            tSpan.err = 1
            id = self.agent.updateStringMeta('ERR')
            value = StringValue(value=stackMap['ERR']['msg'])
            tSpan.exceptionInfo.intValue = id
            tSpan.exceptionInfo.stringValue.CopyFrom(value)

        if 'EXP' in stackMap:
            id = self.agent.updateStringMeta('EXP')
            value = StringValue(value=stackMap['EXP'])
            tSpan.exceptionInfo.intValue = id
            tSpan.exceptionInfo.stringValue.CopyFrom(value)


        if 'clues' in stackMap:
            for annotation in stackMap['clues']:  # list
                id, value = annotation.split(':', 1)
                ann = PAnnotation(key=int(id), value=PAnnotationValue(stringValue=value))
                tSpan.annotation.append(ann)

        try:
            value = None
            if 'NP' in stackMap:  ## nginx
                value = PLongIntIntByteByteStringValue()
                arr = ThriftProtocolUtil._parseStrField(stackMap['NP'])
                value.intValue1 = 2
                if 'D' in arr:
                    value.intValue2 = ThriftProtocolUtil._parseDotFormat(arr['D'])
                if 't' in arr:
                    value.longValue = ThriftProtocolUtil._parseDotFormat(arr['t'])
            elif 'AP' in stackMap:  ## apache
                value = PLongIntIntByteByteStringValue()
                arr = ThriftProtocolUtil._parseStrField(stackMap['AP'])
                value.intValue1 = 3
                if 'i' in arr:
                    value.byteValue1 = int(arr['i'])
                if 'b' in arr:
                    value.byteValue2 = int(arr['b'])
                if 'D' in arr:
                    value.intValue2 = int(arr['D'])
                if 't' in arr:
                    value.longValue = int(int(arr['t']) / 1000)
            if value is not None:
                ann = PAnnotation(key=PROXY_HTTP_HEADER, value=PAnnotationValue(longIntIntByteByteStringValue=value))
                tSpan.annotation.append(ann)
        except Exception as e:
            TCLogger.error("input is illegal,Exception %s", e)

        return tSpan

    def createSpanEvent(self, stackMap):
        assert 'name' in stackMap, " spanEvent should contain a name field"
        spanEv = PSpanEvent()
        spanEv.apiId = self.agent.updateApiMeta(stackMap['name'])

        if 'EXP' in stackMap:
            id = self.agent.updateStringMeta('EXP')
            value = StringValue(value=stackMap['EXP'])
            spanEv.exceptionInfo.intValue = id
            spanEv.exceptionInfo.stringValue.CopyFrom(value)

        nextEv = spanEv.nextEvent
        msgEv = nextEv.messageEvent

        if 'dst' in stackMap:
            msgEv.destinationId = stackMap['dst']

        if 'nsid' in stackMap:
            msgEv.nextSpanId = int(stackMap['nsid'])

        if 'server' in stackMap:
            msgEv.endPoint = stackMap['server']

        if 'S' in stackMap:
            spanEv.startElapsed = int(stackMap['S'])

        if 'E' in stackMap:
            spanEv.endElapsed = int(stackMap['E'])

        if 'stp' in stackMap:
            spanEv.serviceType = int(stackMap['stp'])
        else:
            spanEv.serviceType = PHP_METHOD_CALL

        if 'clues' in stackMap:
            for annotation in stackMap['clues']:  # list
                id, value = annotation.split(':', 1)
                ann = PAnnotation(key=int(id), value=PAnnotationValue(stringValue=value))
                spanEv.annotation.append(ann)
        # todo SQL not support cache bindValue
        if 'SQL' in stackMap:
            id = self.agent.updateSqlMeta(stackMap['SQL'])
            sqlValue = PIntStringStringValue(intValue=id)
            ann = PAnnotation(key=SQL_ID, value=PAnnotationValue(intStringStringValue=sqlValue))
            spanEv.annotation.append(ann)

        return spanEv

    def attachSpanEvent(self, tSpan, span_event):
        tSpan.spanEvent.append(span_event)

    def setSequenceid(self, span_ev, id):
        span_ev.sequence = id

    def setDepth(self, span_ev, depth):
        span_ev.depth = depth
