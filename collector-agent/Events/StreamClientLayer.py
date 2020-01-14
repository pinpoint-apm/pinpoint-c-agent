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

from __future__ import print_function

import errno

from gevent import Timeout
from gevent import socket as asy_socket
from gevent.event import Event  ## write signal

from Common.Logger import TCLogger
from Events.TrainsLayer import *


class StreamClientLayer(TrainLayer):
    def __init__(self,remote,recv_msg_cb,hello_cb,auto_recover=True,timeout= 5):
        '''

        :param tuple remote:
        :param  recv_msg_cb:
        :param hello_cb:
        :param auto_recover:
        :param timeout:
        '''
        super(StreamClientLayer, self).__init__(remote, recv_msg_cb, auto_recover, timeout)
        self.RECV_BUF_SIZE = 4096
        self.recvBuf = bytearray(self.RECV_BUF_SIZE)
        self.pRBufStart = memoryview(self.recvBuf)
        self.hello_cb = hello_cb
        self.rest_data_sz = 0
        self.peer     = ()
        self._read_watcher = None
        self._write_watcher = None
        self.loop = get_hub().loop
        self.timer = self.loop.timer(timeout)
        self.send_buf =b''


    def close(self):
        TCLogger.warning("close connection (%s)", str(self.remote))
        self._read_watcher.stop()
        self._write_watcher.stop()
        self.timer.stop()
        self.socket.close()
        self.state = E_CLOSED
        ## reset send buffer
        self.send_buf = b''
        self.timer =  self.loop.timer(self.timeout)
        self.timer.start(self._connectPeerWithHello,self.remote)


    def _connected(self, addr):
        self.peer = addr
        self.__rawFd = self.socket.fileno()
        TCLogger.info("connect peer: [%s:%d] success" % (addr[0], addr[1]))
        self.state= E_CONNECTED

        ### start watcher recv
        self._enableRead()

        ### stop write
        self._stopWrite()

        ### stop timer
        self.timer.stop()

        ### say hello
        if self.hello_cb is not None:
            self.hello_cb(self)


    def _stopWrite(self):
        self._write_watcher.stop()
        self.state |= E_WRITING

    def _enableRead(self):
        self.state |=  E_READ
        self._read_watcher.start(self._start_recv)

    def _actionTimeOut(self,time):
        if self.state == E_CONNETING:
            TCLogger.info("connecting state:%d timeout:%ds",self.state,time)
            self.close()
        elif self.state & E_WRITING == E_WRITING:
            TCLogger.info("waiting writing %d timeout:%ds", self.state, time)
        else:
            TCLogger.error("unhandled state:%d",self.state)

    def _connectPeerAsy(self, addr, time):
        self.socket = asy_socket.socket(asy_socket.AF_INET, asy_socket.SOCK_STREAM)
        self.socket.setblocking(False)
        self._read_watcher = self.loop.io(self.socket.fileno(), 1)
        self._write_watcher = self.loop.io(self.socket.fileno(), 2)
        try:
            self.state = E_CONNETING
            self.socket.connect(addr)
        except  asy_socket.error as e:
            if not( e.errno in (errno.EINPROGRESS, errno.EWOULDBLOCK)):
                TCLogger.warning("connect:%s error:%s" % (addr, str(e)))
                self.close()
                return False
        self.timer = self.loop.timer(self.timeout)
        self.timer.start(self._actionTimeOut,time)
        self._write_watcher.start(self._connected, addr)
        return True

    def _connectPeerWithHello(self, remote):
        TCLogger.info("try to connect:%s with timeout:%d",str(remote),self.timeout)
        self._connectPeerAsy(remote, self.timeout)

    def _recvFromPeer(self):
        recv_buf  = self.pRBufStart[self.rest_data_sz:]
        recv_total = self.rest_data_sz
        assert recv_total < self.RECV_BUF_SIZE
        try:
             recv_total = self.socket.recv_into(recv_buf,self.RECV_BUF_SIZE - recv_total)
             if recv_total == 0:
                 TCLogger.warning("peer: [%s:%d] closed",self.peer[0],self.peer[1])
                 return -1
        except asy_socket.error as error:
            if error.errno in [errno.EAGAIN]:
                return 0
            TCLogger.warning("peer error:%s ",error)
            return -1
        return recv_total

    def _start_recv(self):
        rLen = self._recvFromPeer()
        if rLen > 0:
            self.rest_data_sz = self.in_msg_cb(self, self.pRBufStart, rLen)
            if self.rest_data_sz > 0:
                self.pRBufStart[0:self.rest_data_sz] = self.pRBufStart[rLen - self.rest_data_sz:rLen]
        elif rLen < 0:
            self.close()

    def start(self):
        try:
            ## start
            self._connectPeerWithHello(self.remote)
        except  Timeout as t:
            TCLogger.warning("remote:%s time out state:%d",str(self.remote),self.state)
            self.close()

    def  _sendData(self):
        try:
            ret = self.socket.send(self.send_buf)
        except asy_socket.error as e:
            TCLogger.error("_sendData  %s",e)
            self.close()
            return
        if ret != len(self.send_buf):
            self.send_buf = self.send_buf[ret:]
            self.sendData()
            return
        TCLogger.debug("send %d",ret)
        ## write is done
        self.state &=(~E_WRITING)
        self._write_watcher.stop()
        self.send_buf = b''


    def sendData(self, outData=None):

        if self.state in (E_CLOSED,E_CONNETING):
            # self.write_wait_queue.append(data)
            TCLogger.debug("current state is:%d, data drop",self.state)
        else:
            if outData is not None:
                self.send_buf+=outData
            self._write_watcher.start(self._sendData)


    def getRawSocketFd(self):
        return self.__rawFd


def _test_hello_cb(n_layer):
    TCLogger.debug("send hello")
    n_layer.sendData(b"GET index.html \r\n\r\n")

def _test_in_msg_cb(self,barray, size):
    '''

    :param memoryview barray:
    :param size:
    :return:
    '''
    print( barray[0:size].tobytes())
    return 1

if __name__ == '__main__':
    tcpLayer = StreamClientLayer(("www.google.com", 80), _test_in_msg_cb, _test_hello_cb)

    tcpLayer.start()
    g = Event()
    g.wait()
