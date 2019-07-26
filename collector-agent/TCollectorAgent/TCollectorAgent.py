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
import time

import gevent

from APIMeta import *
from AgentStateManager import AgentStateManager
from CollectorAgentConf import CollectorAgentConf
from PinpointAgent.PinpointAgent import PinpointAgent
from TCGenerator import *
from Trains import *


class ThriftAgent(PinpointAgent):
    PingCount= 0
    ReqCount = 0
    Negotiation=0
    Clear =1

    def __init__(self,ac):
        '''

        :param CollectorAgentConf ac:
        '''
        self.ac     = ac
        self.tcpHost   =  (ac.CollectorTcpIp, ac.CollectorTcpPort)
        self.statHost  =  (ac.CollectorStatIp, ac.CollectorStatPort)
        self.spanHost  =  (ac.CollectorSpanIp, ac.CollectorSpanPort)

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
        self.startTimeStamp = ac.startTimestamp
        self.agentId = ac.AgentID
        self.agentName= ac.ApplicationName
        self.agentInfo = TAgentInfo(
            agentId =ac.AgentID,
            applicationName= ac.ApplicationName,
            agentVersion= ac.version,
            startTimestamp= self.startTimeStamp,
            serviceType=PHP,
            pid=os.getpid()
        )
        self.interceptManger = InterceptManager(self.sendMsgToCollector,ac)
        self.agentState = AgentStateManager(self.agentId, self.startTimeStamp,self.statHost)
        self.postponed_queue = []
        self.scanLocalInfo()

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

    def upLoadTSpanEvent(self, client,type, data):
        '''
        id fd of client
        :param bytearray data:
        :return:
        '''
        # json_str = json.dumps(jStack)
        stack = json.loads(data.decode('utf-8'))
        TCLogger.info("%s",stack)
        convert = ConvertSpan(stack,self.interceptManger)

        tSpan = convert.toSpan()

        body = CollectorPro.obj2bin(tSpan,SPAN)
        # packet = Packet(PacketType.HEADLESS, len(body), body)
        self.spanLayer.sendData(body)
        # self.spanLayer.sendData(packet.getSerializedData())
        TCLogger.debug("send TSpan:%s",tSpan)


    def handlePHPAgentData(self,client,type,body):
        # if type == 1:
        #     self.handleGetAgentInfo(client,type,body)
        # else:
        self.upLoadTSpanEvent(client,type,body)



    def scanLocalInfo(self):
        ah = AgentHost()
        self.agentInfo.hostname = ah.hostname
        self.agentInfo.ip = ah.ip
        self.agentInfo.ports = ah.port

    def startAll(self):
        self.spanLayer.start()
        self.tcpLayer.start()


    def stop(self):
        self.ac.clean()


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
        ThriftAgent.PingCount+= 1
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
        TCLogger.debug("send ping len:%d", len(ping_data))

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


    def tellWhoAMI(self):
        return {
            "time":str(self.startTimeStamp),
            "id":self.agentId,
            "name":self.agentName
            # "shared_mem_file":self.ac.sharedObjectAddress
        }

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
                    self.agentInfo.startTimestamp)

        packet = Packet(PacketType.CONTROL_HANDSHAKE,
                        0, cmp.getDataLen(),
                        cmp.getBinData())
        return packet

    def __getTAgentInfoPacket(self):
        # self.agentInfo
        TCLogger.debug("agent:%s",self.agentInfo)
        body = CollectorPro.obj2bin(self.agentInfo, AGENT_INFO)
        packet = Packet(PacketType.APPLICATION_REQUEST,CollectorPro.getCurReqCount(),len(body),body)
        ThriftAgent.ReqCount +=1

        return packet




if __name__ == '__main__':
    ac = CollectorAgentConf(CAConfig)
    agent = ThriftAgent(ac )
    # agent.setAgentInfo('10.34.130.79','10.34.130.79')
    # rawSpan='{"name":"a","args":"1,2,3","calls":[{"name":"b","args":"1"},{"name":"c","args":"1"},{"name":"d","args":"1"}]}'
    # agent.upLoadTSpanEvent(rawSpan)
    agent.startAll()
    while True:
        stime = int(time.time()*1000)
        rawSpan='{"name":"PHP Request",' \
                '"server":"10.34.130.79:28081",' \
                '"sid":"3345567788","psid":"3345567789","tid":"test-agent^1560951035971^1",'\
                '"S":%d,"E":20,' \
                '"clues":["46:200"],' \
                '"uri":"/index.html",' \
                '"EC":1, "estr":"DIY",' \
                '"calls":[{"name":"hello","S":0,"E":8,"calls":[{"name":"hello2","S":2,"E":2,"clues":["-1:null","14:2019/06/25"],"calls":[{"name":"hello3","S":4,"E":4}]}]}],' \
                '"client":"10.10.10.10"}'% (stime)

        agent.upLoadTSpanEvent(None,1,rawSpan.encode())
        gevent.sleep(10)

    g = Event()
    g.wait()

    # g1 = gevent.spawn(
    # time.sleep(1)
    # rawSpan='{"name":"a","args":"1,2,3","calls":[{"name":"b","args":"1"},{"name":"c","args":"1"},{"name":"d","args":"1"}]}'
    # agent.upLoadTSpanEvent(rawSpan)
    # g1.join()


