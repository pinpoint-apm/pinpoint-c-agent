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
# import json
import os
# import struct
# import time
#
# import gevent

from CollectorAgent.TPackets import ControlMessageDecoder, ControlMessage, HandShakeMessage
from CollectorAgent.ThriftSpanFactory import ThriftSpanFactory
from Common.AgentHost import AgentHost
from PinpointAgent.PinpointAgent import PinpointAgent
from PinpointAgent.Type import PHP, API_DEFAULT, AgentSocketCode
from Events import *
from CollectorAgent.TCGenerator import *
from CollectorAgent.APIMeta import *
from CollectorAgent.AgentStateManager import AgentStateManager


class ThriftAgentImplement(PinpointAgent):
    PingCount= 0
    ReqCount = 0


    def __init__(self,ac,app_id,app_name,serviceType=PHP):

        super().__init__(app_id,app_name)
        self.ac         = ac
        self.tcpHost    =  (ac.CollectorAgentIp, ac.CollectorAgentPort)
        self.statHost   =  (ac.CollectorStatIp, ac.CollectorStatPort)
        self.spanHost   =  (ac.CollectorSpanIp, ac.CollectorSpanPort)
        TCLogger.debug("CollectorTcp %s CollectorStat %s CollectorSpan %s" % (self.tcpHost, self.statHost, self.spanHost))

        self.tcpLayer = StreamClientLayer(self.tcpHost, self.handlerResponse, self.collectorTcpHello)

        self.spanLayer = DgramLayer(self.spanHost,None)
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
        self.startTimeStamp = self.ac.startTimestamp
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
        self.span_factory = ThriftSpanFactory(self)

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

        tSpan = self.span_factory.make_span(stack)
        body = CollectorPro.obj2bin(tSpan,SPAN)
        # packet = Packet(PacketType.HEADLESS, len(body), body)
        self.spanLayer.sendData(body)
        # self.spanLayer.sendData(packet.getSerializedData())
        TCLogger.debug("send TSpan:%s",tSpan)

    def scanLocalInfo(self):
        ah = AgentHost()
        self.agentInfo.hostname = ah.hostname
        self.agentInfo.ip = ah.ip
        self.agentInfo.ports = self.ac.getWebPort()

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