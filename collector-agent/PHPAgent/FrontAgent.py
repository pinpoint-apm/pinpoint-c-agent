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
# coding:utf-8

import json
import os
import struct

from gevent import socket as gsocket

from Common import *
from PHPAgent.Type import RESPONSE_AGENT_INFO
from PHPAgent.PHPAgentConf import PHPAgentConf
from Trains import StreamServerLayer


class FrontAgent(object):
    HEADERSIZE = 8
    def __init__(self, ac, msgCallback):
        '''

        :param PHPAgentConf ac:
        '''
        self.address =ac.Address

        if os.path.exists(self.address):
            os.remove(self.address)
        self.listen_socket =  gsocket.socket(gsocket.AF_UNIX, gsocket.SOCK_STREAM)
        self.listen_socket.bind(self.address)
        self.listen_socket.listen(256)
        self.server = StreamServerLayer(self.listen_socket, self._recvData, self._phpClientSayHello)
        self.msgHandleCallback = msgCallback
        self.hello_cb = []

    def _recvData(self, client, buf, bsz):
        p = 0

        ###
        if bsz < FrontAgent.HEADERSIZE:
            return bsz

        while p < bsz:
            type, len = struct.unpack('!ii', buf[p:p + FrontAgent.HEADERSIZE].tobytes())
            p += FrontAgent.HEADERSIZE
            if p+len > bsz:
                return p - FrontAgent.HEADERSIZE
            body = buf[p:p + len].tobytes()
            self.msgHandleCallback(client, type, body)
            p+= len

        return 0

    def registerPHPAgentHello(self,hello_cb):
        self.hello_cb.append(hello_cb)

    def _phpClientSayHello(self, client):
        response = {}
        for cb in self.hello_cb:
            data = cb()
            for key in data:
                response[key] = data[key]

        data = json.dumps(response)
        buf  = struct.pack('!ii',RESPONSE_AGENT_INFO,len(data)) + data.encode()
        client.sendData(buf)
        TCLogger.debug("send hello:%d len:%d",client.socket.fileno(),len(buf))


    def start(self):
        self.server.start()

    def stop(self):
        pass



def handleAgentPacket(client,type,str):
    '''

    :param str str:
    :return:
    '''
    TCLogger.debug("%d,%s",type,str)



if __name__ == '__main__':
    ac = PHPAgentConf(CAConfig)
    agent = FrontAgent(ac, handleAgentPacket)
    agent.start()
    from gevent.event import Event
    import gevent,signal
    evt =Event()

    gevent.signal(signal.SIGQUIT, evt.set)
    gevent.signal(signal.SIGTERM, evt.set)
    gevent.signal(signal.SIGINT, evt.set)

    evt.wait()
