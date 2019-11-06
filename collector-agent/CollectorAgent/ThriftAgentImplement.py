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
import json
import os
import struct
import time

import gevent

from CollectorAgent.TPackets import ControlMessageDecoder, ControlMessage, HandShakeMessage
from Common import *
from Common.AgentHost import AgentHost
from PinpointAgent.PinpointAgent import PinpointAgent
from PinpointAgent.Type import PHP, API_DEFAULT, AgentSocketCode, AGENT_INFO, SPAN, PHP_METHOD_CALL, API_WEB_REQUEST, \
    PROXY_HTTP_HEADER
from Proto.Trift.Trace.ttypes import TSpan, TSpanEvent, TIntStringValue, TAnnotation, TAnnotationValue, \
    TLongIntIntByteByteStringValue
from Events import *
from CollectorAgent.TCGenerator import *
from CollectorAgent.APIMeta import *
from CollectorAgent.AgentStateManager import AgentStateManager


class ThriftAgentImplement(PinpointAgent):
    PingCount= 0
    ReqCount = 0


    def __init__(self,manage,ac,app_id,app_name,serviceType=PHP):

        super().__init__(app_id,app_name)
        self.ac         = ac
        self.tcpHost    =  (ac.CollectorTcpIp, ac.CollectorTcpPort)
        self.statHost   =  (ac.CollectorStatIp, ac.CollectorStatPort)
        self.spanHost   =  (ac.CollectorSpanIp, ac.CollectorSpanPort)
        TCLogger.debug("CollectorTcp %s CollectorStat %s CollectorSpan %s" % (self.tcpHost, self.statHost, self.spanHost))

        self.tcpLayer = StreamClientLayer(self.tcpHost, self.handlerResponse, self.collectorTcpHello)

        self.spanLayer = DgramLayer(self.spanHost,None)
        self.sequenceId = 0
        self.packetRoute = {
            PacketType.APPLICATION_SEND : self.handle_default,
            PacketType.APPLICATION_TRACE_SEND : self.handle_default,
            PacketType.APPLICATION_TRACE_SEND_ACK : self.handle_default,
            PacketType.APPLICATION_REQUEST : self.handle_appResponse,
            PacketType.APPLICATION_RESPONSE : self.handle_appliaction_response,
            PacketType.APPLICATION_STREAM_CREATE : self.handle_default,
            PacketType.APPLICATION_STREAM_CREATE_SUCCESS : self.handle_default,
            PacketType.APPLICATION_STREAM_CREATE_FAIL : self.handle_default,
            PacketType.APPLICATION_STREAM_CLOSE : self.handle_default,
            PacketType.APPLICATION_STREAM_PING : self.handle_default,
            PacketType.APPLICATION_STREAM_PONG : self.handle_default,
            PacketType.APPLICATION_STREAM_RESPONSE : self.handle_default,
            PacketType.CONTROL_CLIENT_CLOSE : self.handle_default,
            PacketType.CONTROL_SERVER_CLOSE : self.handle_default,
            PacketType.CONTROL_HANDSHAKE : self.handle_default,
            PacketType.CONTROL_HANDSHAKE_RESPONSE : self.handle_handshake_response,
            PacketType.CONTROL_PING_SIMPLE : self.handle_control_ping,
            PacketType.CONTROL_PONG : self.handle_recv_pong
        }
        self.socketCode = AgentSocketCode.NONE
        self.manage = manage
        self.startTimeStamp = self.manage.startTimestamp
        self.agentName= app_name
        self.agentInfo = TAgentInfo(
            agentId =app_id,
            applicationName= app_name,
            agentVersion= ac.version,
            startTimestamp= self.startTimeStamp,
            serviceType=self.service_type,
            pid=os.getpid()
        )

        self.agentState = AgentStateManager(self.app_id, self.startTimeStamp,self.statHost)
        self.postponed_queue = []
        self.scanLocalInfo()
        self.api_metas = {}
        self.string_metas = {}

    ## expose to other module
    def sendMsgToCollector(self,msg):
        if self.socketCode == AgentSocketCode.NONE: ## channel not ready
            TCLogger.debug("AgentState not ready,postpone size:%d",len(msg))
            self.postponed_queue.append(msg)
        else:
            ### send backlog
            self._flushPostponed()
            self.tcpLayer.sendData(msg)

    def _flushPostponed(self):
        if len(self.postponed_queue) > 0:
            for m in self.postponed_queue:
                TCLogger.debug("postponed size:%d ", len(m))
                self.tcpLayer.sendData(m[:])
            self.postponed_queue = []



    def updateApiMeta(self, name, api_type=API_DEFAULT):

        if name in self.api_metas:
            return self.api_metas[name]
        else:
            meta = APIMeta(name=name, type=api_type,
                           agentStartTime=self.startTimeStamp, agentId=self.app_id,
                           agentName=self.app_name)
            self.sendMsgToCollector(meta.toPacket().getSerializedData())
            self.api_metas[name] = meta
            return meta

    def sendMeta(self, meta):
        '''

        :param APIMeta meta:
        :return:
        '''
        TCLogger.debug("meta: %s", meta.name)
        self.sendMsgToCollector(meta.toPacket().getSerializedData())


    def updateStringMeta(self,name):

        if name in self.string_metas:
            return self.string_metas[name]
        else:

            meta = StringMetaData(agentStartTime =self.startTimeStamp, agentId=self.app_id,name=name)
            self.sendMeta(meta)
            self.string_metas[name] = meta
            return meta

    def sendSpan(self,stack):
        '''

        :param dict stack:
        :return:
        '''
        ### must reset to zero

        self.sequenceId = 0
        tSpan = self.makeSpan(stack)

        body = CollectorPro.obj2bin(tSpan,SPAN)
        # packet = Packet(PacketType.HEADLESS, len(body), body)
        self.spanLayer.sendData(body)
        # self.spanLayer.sendData(packet.getSerializedData())
        TCLogger.debug("send TSpan:%s",tSpan)


    def genSpanEvent(self, span):
        '''

        :param  dict span:
        :return TSpanEvent:
        '''
        assert 'name' in span
        spanEv = TSpanEvent()

        spanEv.apiId = self.updateApiMeta(span['name']).apiId
        spanEv.annotations = []
        if 'EXP' in  span:
            id = self.updateStringMeta('EXP').apiId
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


    def makeSpan(self, stackMap, tSpan=None, index=0):
        '''

        :param stackMap:
        :param tSpan:
        :param index: the depth of call stack
        :param sequenceId: reset from every span. Must start from zero
        :return:
        '''
        if tSpan is None:  ## A TSpan
            tSpan = self.genTspan(stackMap)
        else:  ## A span event
            spanEv = self.genSpanEvent(stackMap)
            # spanEv.spanId    = tSpan.spanId
            spanEv.sequence = self.sequenceId
            self.sequenceId += 1
            spanEv.depth = index
            tSpan.spanEventList.append(spanEv)

        if 'calls' in stackMap:
            for called in stackMap['calls']:
                self.makeSpan(called, tSpan, index + 1)

        return tSpan

    def genTspan(self, span):
        '''
        :param dict span:
        :param InterceptManager interceptManger
        :return :
        '''

        tSpan = TSpan()
        tSpan.apiId = self.updateApiMeta(span['name'], API_WEB_REQUEST).apiId
        tSpan.agentStartTime = self.startTimeStamp

        if 'appid' in span:
            tSpan.agentId = span['appid']
        else:
            tSpan.agentId = self.app_id

        if 'appname' in span:
            tSpan.applicationName = span['appname']
        else:
            tSpan.applicationName = self.app_name

        if 'stp' in span:
            tSpan.serviceType = int(span['stp'])
            tSpan.applicationServiceType = int(span['stp'])
        else:
            tSpan.serviceType = PHP
            tSpan.applicationServiceType = PHP

        if 'psid' in span:
            tSpan.parentSpanId = int(span['psid'])

        if 'tid' in span:
            tSpan.transactionId = TransactionId(encoded_str= span['tid']).getBytes()

        if 'sid' in span:
            tSpan.spanId = int(span['sid'])

        if 'S' in span:
            tSpan.startTime = span['S']

        if 'E' in span:
            tSpan.elapsed = span['E']

        if 'uri' in span:
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
            id = self.updateStringMeta('ERR').apiId
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
                arr = ThriftProtocolUtil._parseStrField(span['NP'])
                value.intValue1 = 2
                if 'D' in arr:
                    value.intValue2 =ThriftProtocolUtil._parseDotFormat(arr['D'])
                if 't' in arr:
                    value.longValue =ThriftProtocolUtil._parseDotFormat(arr['t'])
            elif 'AP' in span: ## apache
                arr = ThriftProtocolUtil._parseStrField(span['AP'])
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

    def scanLocalInfo(self):
        ah = AgentHost(self.ac)
        self.agentInfo.hostname = ah.hostname
        self.agentInfo.ip = ah.ip
        self.agentInfo.ports = ah.port

    def start(self):
        self.tcpLayer.start()
        self.spanLayer.start()


    def stop(self):
        self.ac.clean()
        self.tcpLayer.stop()
        self.spanLayer.stop()


    def handle_default(self, tcp_layer, ptype, header, vBody):
        TCLogger.debug('ptype:%d ignore',ptype)
        return 0


    ## APPLICATION_REQUEST
    def handle_appResponse(self,tcp_layer,ptype,header,vBody):
        TCLogger.debug('ptype:%d ignore', ptype)
        return 0

    def negotiatedDone(self, code, tcp_layer):

        # # register ping

        TrainLayer.registerTimers(self.__sendPing, 10, tcp_layer)
        TCLogger.debug("register send ping ")
        #
        # # register TagentInfo
        TrainLayer.registerTimers(self.__sendAgentInfo, 0, tcp_layer)
        TCLogger.debug(" send agentInfo ")

        # check the postponed

        self._flushPostponed()


    ## CONTROL_HANDSHAKE_RESPONSE
    def handle_handshake_response(self,tcp_layer,ptype,header,vBody):
        cbf = ChannelBufferV2(vBody.tobytes())
        cmg = ControlMessageDecoder.decodingControlMessage(cbf)

        if cmg.type == ControlMessage.CONTROL_MESSAGE_MAP:
            code_CM = cmg.data[b'code']
            subCode_CM = cmg.data[b'subCode']
            if code_CM.type ==  ControlMessage.CONTROL_MESSAGE_LONG and subCode_CM.type == ControlMessage.CONTROL_MESSAGE_LONG:
                code    = code_CM.data
                subCode = subCode_CM.data
                TCLogger.debug("code %d subcode %d",code,subCode)
                self.socketCode = HandShakeMessage.getNextStateCode(code,subCode)
                self.negotiatedDone(self.socketCode, tcp_layer)

        return 0

    def handle_appliaction_response(self,tcp_layer,ptype,header,vBody):
        '''

        :param tcp_layer:
        :param ptype:
        :param header:
        :param memoryview vBody:
        :return:
        '''
        type,obj = CollectorPro.bin2obj(vBody)
        TCLogger.debug("response: %s",obj)


    def __getPingPacket(self):
        ## ping context

        pingBuf = struct.pack("!ibb",self.socketCode,#ThriftAgent.PingCount,
                              0,
                              self.socketCode)

        packet = Packet(hType = PacketType.CONTROL_PING_PAYLOAD,body=pingBuf)
        ThriftAgentImplement.PingCount+= 1
        return packet





    ## PacketType.CONTROL_PONG
    def handle_recv_pong(self,tcp_layer,ptype,header,vBody):
        TCLogger.debug("recv pong")
        return 0

    ## PacketType.CONTROL_PING
    def handle_control_ping(self,tcp_layer,ptype,header,vBody):
        ping = Packet(PacketType.CONTROL_PONG)
        tcp_layer.sendData(ping.getSerializedData())
        return 0


    def handlerResponse(self,tcp_layer, view, size):
        '''

        :param memoryview view:
        :param int size:
        :return:
        '''
        offset  = 0
        iterPacket = Packet.parseNetByteStream(view, size)
        for packet in iterPacket:
            offset,ptype,header,vBody = packet
            self.packetRoute[ptype](tcp_layer,ptype,header,vBody)

        return 0

    def __sendPing(self,tcp_layer):
        ping_data = self.__getPingPacket().getSerializedData()
        tcp_layer.sendData(ping_data)
        # TCLogger.debug("send ping len:%d", len(ping_data))

    def __sendAgentInfo(self,tcp_layer):
        agent_data = self.__getTAgentInfoPacket().getSerializedData()
        tcp_layer.sendData(agent_data)
        TCLogger.debug("send agentInfo len:%d", len(agent_data))

    def collectorTcpHello(self,tcp_layer):
        '''
        :param StreamClientLayer tcp_layer:
        :return:
        '''
        ### handshakepacket
        data = self.__getHandShakePacket(tcp_layer).getSerializedData()
        tcp_layer.sendData(data)
        TCLogger.debug("send hand shake len:%d",len(data))

    def __getHandShakePacket(self,tcp_layer):
        cmp = HandShakeMessage(
                    tcp_layer.getRawSocketFd(),
                    self.agentInfo.hostname,
                    True,
                    self.agentInfo.ip,
                    self.agentInfo.agentId,
                    self.agentInfo.applicationName,
                    PHP,
                    self.agentInfo.pid,
                    self.agentInfo.agentVersion,
                    self.startTimeStamp)

        packet = Packet(PacketType.CONTROL_HANDSHAKE,
                        0, cmp.getDataLen(),
                        cmp.getBinData())
        return packet

    def __getTAgentInfoPacket(self):
        # self.agentInfo
        TCLogger.debug("agent:%s",self.agentInfo)
        body = CollectorPro.obj2bin(self.agentInfo, AGENT_INFO)
        packet = Packet(PacketType.APPLICATION_REQUEST,CollectorPro.getCurReqCount(),len(body),body)
        ThriftAgentImplement.ReqCount +=1

        return packet