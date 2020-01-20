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
from gevent import get_hub
from gevent import socket as asy_socket

from Common import *
from Common.Logger import TCLogger


class StreamServerLayer(object):

    class Client:
        def __init__(self,socket,readEv,writeEv):
            self.socket = socket
            self._read_watcher = readEv
            self._write_watcher = writeEv
            self.RECV_BUF_SIZE = 4096
            self.recvBuf = bytearray(self.RECV_BUF_SIZE)
            self.pRBufStart = memoryview(self.recvBuf)
            self.rest_data_sz = 0
            self.send_buf = b''

        def recv(self,in_msg_cb):
            '''

            :param self.Client client:
            :return: if rLen < 0 error happens
            '''
            rLen = self._recvFromPeer()
            if rLen > 0:
                self.rest_data_sz = in_msg_cb(self, self.pRBufStart, rLen)
                if self.rest_data_sz > 0:
                    self.pRBufStart[0:self.rest_data_sz] = self.pRBufStart[rLen - self.rest_data_sz:rLen]
            return rLen

        def _recvFromPeer(self):
            recv_buf = self.pRBufStart[self.rest_data_sz:]
            recv_total = self.rest_data_sz
            assert recv_total < self.RECV_BUF_SIZE
            try:
                recv_total = self.socket.recv_into(recv_buf, self.RECV_BUF_SIZE - recv_total)
                if recv_total == 0:
                    TCLogger.warning("peer close socket");
                    return -1
            except asy_socket.error as error:
                if error.errno in [asy_socket.EAGAIN, asy_socket.EWOULDBLOCK]:
                    return 0
                TCLogger.warning("peer error:%s ", error)
                return -1
            return recv_total+self.rest_data_sz

        def close(self):
            self.socket.close()
            if self._read_watcher is not None:
                self._read_watcher.stop()
            if self._write_watcher is not None:
                self._write_watcher.stop()

        def _sendData(self):
            try:
                ret = self.socket.send(self.send_buf)
            except asy_socket.error as e:
                TCLogger.error("_sendData  %s", e)
                self.close()
                return
            ### not ready try next time
            if ret != len(self.send_buf):
                self.send_buf = self.send_buf[ret:]
                # self.sendData()
                return
            TCLogger.debug("send %d", ret)
            self._write_watcher.stop()

        def sendData(self,data):
            self.send_buf += data
            self._write_watcher.start(self._sendData)


    def __init__(self, listen_sock, in_msg_cb,hello_cb):
        self.listen = listen_sock
        self.in_msg_cb = in_msg_cb
        self.say_hello_cb = hello_cb
        self.listen_event = None
        self.loop = get_hub().loop
        self.clients ={}

    def start(self):
        self._startAccept()

    def _startAccept(self):
        self.listen_event = self.loop.io(self.listen.fileno(), 1)
        self.listen_event.start(self._listenhandler)

    def _listenhandler(self):
        try:
            sock = self.listen
            # client_socket
            client_socket, address = sock.accept()
            client_socket.setblocking(False)
            # sock = gsocket(_socket = client_socket)
        except asy_socket.error as error:
            TCLogger.warning("accept:%s",error)
            return

        TCLogger.info("new connect fd:%s ",client_socket.fileno())
        client_in_event= self.loop.io(client_socket.fileno(), 1)
        client_out_event =  self.loop.io(client_socket.fileno(), 2)

        client = self.Client(client_socket,client_in_event,client_out_event)

        client_in_event.start(self.handleClientRead, client)

        ## say hello

        if self.say_hello_cb is not None:
            self.say_hello_cb(client)

        ## (socket,read_cv,write_cb)
        self.clients[client_socket.fileno()] = client

    def handleClientRead(self, client):
        '''

        :param self.Client client:
        :return:
        '''
        while True:
            len = client.recv(self.in_msg_cb)
            if len > 0:## read again
                continue
            if len < 0:
                TCLogger.warning(" client:%d disconnected",client.socket.fileno())
                fd = client.socket.fileno()
                client.close()
                del self.clients[fd]
            break


def msgHandle(client,buffer,blen):
    '''

    :param gsocket socket:
    :return:
    '''
    print(buffer[0:blen].tobytes())
    return 0

if __name__ == '__main__':
    import  os
    address = '/tmp/collector-agent.sock'

    if os.path.exists(address):
        os.remove(address)

    listen_socket = asy_socket.socket(asy_socket.AF_UNIX, asy_socket.SOCK_STREAM)
    listen_socket.bind(address)
    listen_socket.listen(256)
    server = StreamServerLayer(listen_socket,msgHandle,None)
    from gevent.event import Event
    server.start()
    g =Event()
    g.wait()
