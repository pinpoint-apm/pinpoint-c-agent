#-------------------------------------------------------------------------------
# Copyright 2019 NAVER Corp
# 
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License.  You may obtain a copy
# of the License at
# 
#   http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations under
# the License.
#-------------------------------------------------------------------------------
#!/usr/bin/env python
# -*- coding: UTF-8 -*-


# keyWord ={
#     1:'agentId',
#     2:'applicationName',
#     3:'agentStartTime',
#     4:'transactionId',
#     5:'spanId',
#     6:'parentSpanId',
#     7:'spanId',
#     8:'parentSpanId ',
#     9:'startTime',
#     10:'elapsed ',
#     11:'rpc',
#     12:'serviceType',
#     13:'endPoint',
#     14:'remoteAddr',
#     15:'annotations',
#     16:'flag',
#     17:'err',
#     18:'spanEventList',
#     19:'parentApplicationName',
#     20:'parentApplicationType',
#     21:'acceptorHost',
#     25:'apiId',
#     26:'exceptionInfo',
#     30:'pplicationServiceType',
#     31:'loggingTransactionInfo',
#     32:'key',
#     33:'value'
# }

# from Intercept import interceptManger
from Common import TCLogger
from TCollectorAgent.Protocol import *
from Common.Trace.ttypes import TAnnotation, TSpanEvent, TAnnotationValue, TIntStringValue,TLongIntIntByteByteStringValue


class TCGenerator(object):

    @staticmethod
    def fetchIfExist(jMap,name):
        '''

        :param dict jMap:
        :param name:
        :return:
        '''
        if jMap.has_key(name):
            return jMap[name]
        return None


    @staticmethod
    def __transactionIDHelper(interManger,id):
        '''

        :param InterceptManager interManger:
        :param int id:
        :return:
        '''

        # return str(interManger.ac.AgentID)+'^'+str(interManger.startTimeStamp)+'^'+str(id)


class ConvertSpan(object):
    def __init__(self,stackMap,interceptManger):
        '''

        :param dict stackMap:
        '''
        self.stackMap = stackMap
        self.depth = 0
        self.sequenceId = 0
        self.interceptManger = interceptManger

    def toSpan(self):
        return self.makeSpan(self.stackMap)


    def makeSpan(self,stackMap,tSpan = None,index = 0):

        if tSpan is None:  ## A TSpan
            tSpan            = self.genTspan(stackMap,self.interceptManger)
        else:                ## A span event
            spanEv           = self.genSpanEvent(stackMap,self.interceptManger)
            # spanEv.spanId    = tSpan.spanId
            spanEv.sequence  = self.sequenceId
            self.sequenceId += 1
            spanEv.depth     = index
            tSpan.spanEventList.append(spanEv)

        if 'calls' in stackMap:
            for called in stackMap['calls']:
                self.makeSpan(called,tSpan,index +1 )

        return tSpan


    def genTspan(self, span, interceptManger):
        '''

        :param dict span:
        :param InterceptManager interceptManger
        :return :
        '''

        tSpan = TSpan()
        tSpan.apiId = interceptManger.updateApiMeta(span['name'], API_WEB_REQUEST).apiId
        tSpan.spanId = CollectorPro.createSpanid()
        tSpan.applicationName = interceptManger.ac.ApplicationName
        tSpan.agentStartTime = interceptManger.startTimeStamp
        tSpan.agentId = interceptManger.ac.AgentID

        if 'stp' in span:
            tSpan.serviceType = int(span['stp'])
            tSpan.applicationServiceType = int(span['stp'])
        else:
            tSpan.serviceType = PHP
            tSpan.applicationServiceType = PHP

        if 'psid' in span:
            tSpan.parentSpanId = int(span['psid'])

        if 'tid' in span:
            tSpan.transactionId = TransactionId(encoded_str=  span['tid']).getBytes()

        if 'sid' in span:
            tSpan.spanId = int(span['sid'])

        if 'S' in span:
            tSpan.startTime = span['S']

        if 'E' in span:
            tSpan.elapsed = span['E']

        if 'uri' in  span:
            tSpan.rpc = span['uri']

        if 'pname' in span:
            tSpan.parentApplicationName = span['pname']

        if 'ptype' in span:
            tSpan.parentApplicationType = int(span['ptype'])

        if 'client' in span:
            tSpan.remoteAddr = span['client']

        if 'server' in span:
            tSpan.endPoint = span['server']

        if 'ERR' in span:
            tSpan.err = 1
            id = interceptManger.updateStringMeta('ERR').apiId
            tSpan.exceptionInfo = TIntStringValue(id,span['ERR']['msg'])

        if 'Ah' in span:
            tSpan.acceptorHost = span['Ah']

        tSpan.spanEventList = []
        tSpan.annotations = []

        if 'clues'  in span:
            for annotation in span['clues']:  # list
                id, value = annotation.split(':', 1)
                ann = TAnnotation(int(id), TAnnotationValue(stringValue=value))
                tSpan.annotations.append(ann)

        try:
            value = TLongIntIntByteByteStringValue()
            if 'NP' in span: ## nginx
                arr = ConvertSpan._parseStrField(span['NP'])
                value.intValue1 = 2
                if 'D' in arr:
                    value.intValue2 =ConvertSpan._parseDotFormat(arr['D'])
                if 't' in arr:
                    value.longValue =ConvertSpan._parseDotFormat(arr['t'])
            elif 'AP' in span: ## apache
                arr = ConvertSpan._parseStrField(span['AP'])
                value.intValue1 = 3
                if 'i' in arr:
                    value.byteValue1 = int(arr['i'])
                if 'b' in arr:
                    value.byteValue2 = int(arr['b'])
                if 'D' in arr:
                    value.intValue2  = int(arr['D'])
                if 't' in arr:
                    value.longValue  = int(int(arr['t'])/1000)
            ann = TAnnotation(PROXY_HTTP_HEADER,TAnnotationValue(longIntIntByteByteStringValue=value))
            tSpan.annotations.append(ann)
        except Exception as e:
            TCLogger.error("input is illegal,Exception %s",e)

        return tSpan

    @staticmethod
    def _parseStrField(str):
        '''

        :param string str:
        :return dict:
        '''
        ret = {}
        for item in str.split():
            key,value = item.split(sep='=')
            ret[key] = value
        return ret

    @staticmethod
    def _parseDotFormat(time):
        '''

        :param string time:
        :return:
        '''
        sec, ms = time.split(sep='.')
        return int(sec)*1000+int(ms)


    def genSpanEvent(self, span, interceptManger):
        '''

        :param  dict span:
        :return TSpanEvent:
        '''
        assert 'name' in span
        spanEv = TSpanEvent()


        spanEv.apiId = interceptManger.updateApiMeta(span['name']).apiId
        spanEv.annotations = []
        if 'EXP' in  span:
            id = interceptManger.updateStringMeta('EXP').apiId
            spanEv.exceptionInfo = TIntStringValue(id, span['EXP'])

        if 'dst' in  span:
            spanEv.destinationId = span['dst']

        if 'S' in  span:
            spanEv.startElapsed = span['S']

        if 'E' in span:
            spanEv.endElapsed   = span['E']

        if 'dst' in span:
            spanEv.destinationId = span['dst']

        if 'nsid' in span:
            spanEv.nextSpanId   = int(span['nsid'])

        if 'stp' in span:
            spanEv.serviceType  = int(span['stp'])
        else:
            spanEv.serviceType = PHP_METHOD_CALL

        if 'clues' in span:
            for annotation in span['clues']:  # list
                id, value = annotation.split(':', 1)

                if value and value[0] =='[': ## value is a In
                    pass
                else: ## value is a string
                    ann = TAnnotation(int(id), TAnnotationValue(stringValue=value))
                    spanEv.annotations.append(ann)

        return spanEv
