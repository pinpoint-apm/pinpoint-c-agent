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
from CollectorAgent.Protocol import TransactionId
from CollectorAgent.TCGenerator import ThriftProtocolUtil
from Common.Logger import TCLogger
from PHPAgent.SpanFactory import SpanFactory
from PinpointAgent.Type import API_WEB_REQUEST, PHP, PROXY_HTTP_HEADER, PHP_METHOD_CALL
from Proto.Trift.Trace.ttypes import TSpan, TIntStringValue, TAnnotation, TAnnotationValue, \
    TLongIntIntByteByteStringValue, TSpanEvent


class ThriftSpanFactory(SpanFactory):
    def create_span(self,stackMap):
        tSpan = TSpan()
        tSpan.apiId = self.agent.updateApiMeta(stackMap['name'], API_WEB_REQUEST).apiId
        tSpan.agentStartTime = self.agent.startTimeStamp

        if 'appid' in stackMap:
            tSpan.agentId = stackMap['appid']
        else:
            tSpan.agentId = self.agent.app_id

        if 'appname' in stackMap:
            tSpan.applicationName = stackMap['appname']
        else:
            tSpan.applicationName = self.agent.app_name

        if 'stp' in stackMap:
            tSpan.serviceType = int(stackMap['stp'])
            tSpan.applicationServiceType = int(stackMap['stp'])
        else:
            tSpan.serviceType = PHP
            tSpan.applicationServiceType = PHP

        if 'psid' in stackMap:
            tSpan.parentSpanId = int(stackMap['psid'])

        if 'tid' in stackMap:
            tSpan.transactionId = TransactionId(encoded_str=stackMap['tid']).getBytes()

        if 'sid' in stackMap:
            tSpan.spanId = int(stackMap['sid'])

        if 'S' in stackMap:
            tSpan.startTime = stackMap['S']

        if 'E' in stackMap:
            tSpan.elapsed = stackMap['E']

        if 'uri' in stackMap:
            tSpan.rpc = stackMap['uri']

        if 'pname' in stackMap:
            tSpan.parentApplicationName = stackMap['pname']

        if 'ptype' in stackMap:
            tSpan.parentApplicationType = int(stackMap['ptype'])

        if 'client' in stackMap:
            tSpan.remoteAddr = stackMap['client']

        if 'server' in stackMap:
            tSpan.endPoint = stackMap['server']

        if 'ERR' in stackMap:
            tSpan.err = 1
            id = self.agent.updateStringMeta('ERR').apiId
            tSpan.exceptionInfo = TIntStringValue(id, stackMap['ERR']['msg'])

        if 'Ah' in stackMap:
            tSpan.acceptorHost = stackMap['Ah']

        tSpan.spanEventList = []
        tSpan.annotations = []

        if 'clues' in stackMap:
            for annotation in stackMap['clues']:  # list
                id, value = annotation.split(':', 1)
                ann = TAnnotation(int(id), TAnnotationValue(stringValue=value))
                tSpan.annotations.append(ann)

        try:
            value = TLongIntIntByteByteStringValue()
            if 'NP' in stackMap:  ## nginx
                arr = ThriftProtocolUtil._parseStrField(stackMap['NP'])
                value.intValue1 = 2
                if 'D' in arr:
                    value.intValue2 = ThriftProtocolUtil._parseDotFormat(arr['D'])
                if 't' in arr:
                    value.longValue = ThriftProtocolUtil._parseDotFormat(arr['t'])
            elif 'AP' in stackMap:  ## apache
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
            ann = TAnnotation(PROXY_HTTP_HEADER, TAnnotationValue(longIntIntByteByteStringValue=value))
            tSpan.annotations.append(ann)
        except Exception as e:
            TCLogger.error("input is illegal,Exception %s", e)

        return tSpan

    def create_span_event(self,stackMap):
        assert 'name' in stackMap
        spanEv = TSpanEvent()

        spanEv.apiId = self.agent.updateApiMeta(stackMap['name']).apiId
        spanEv.annotations = []
        if 'EXP' in stackMap:
            id = self.agent.updateStringMeta('EXP').apiId
            spanEv.exceptionInfo = TIntStringValue(id, stackMap['EXP'])

        if 'dst' in stackMap:
            spanEv.destinationId = stackMap['dst']

        if 'S' in stackMap:
            spanEv.startElapsed = stackMap['S']

        if 'E' in stackMap:
            spanEv.endElapsed = stackMap['E']

        if 'dst' in stackMap:
            spanEv.destinationId = stackMap['dst']

        if 'nsid' in stackMap:
            spanEv.nextSpanId = int(stackMap['nsid'])

        if 'stp' in stackMap:
            spanEv.serviceType = int(stackMap['stp'])
        else:
            spanEv.serviceType = PHP_METHOD_CALL

        if 'clues' in stackMap:
            for annotation in stackMap['clues']:  # list
                id, value = annotation.split(':', 1)

                if value and value[0] == '[':  ## value is a In
                    pass
                else:  ## value is a string
                    ann = TAnnotation(int(id), TAnnotationValue(stringValue=value))
                    spanEv.annotations.append(ann)

        return spanEv

    def attach_span_event(self,tSpan,span_event):
        tSpan.spanEventList.append(span_event)

    def set_sequenceid(self,span_ev,id):
        span_ev.sequence = id

    def set_depth(self,span_ev,depth):
        span_ev.depth = depth