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

from gevent import socket as asy_socket

from Trains.TrainsLayer import *


class DgramLayer(TrainLayer):

    def start(self):
        self.__socket = asy_socket.socket(asy_socket.AF_INET,asy_socket.SOCK_DGRAM)
        ### no need to recv
        self.__socket.connect(self.remote)
        self.state = E_CONNECTED
        # if self.in_msg_cb is None:
        #     return
        # not support recv
        # while self.state != E_CLOSED:
        #     ret = self.__socket.recv(8192)



    def sendData(self, data):
        self.__socket.send(data)

    def stop(self):
        self.__socket.close()



if __name__ == '__main__':
    dg = DgramLayer(('10.34.135.162',8000),None)
    dg.start()
    dg.sendData("fasdfasdfasdf")
