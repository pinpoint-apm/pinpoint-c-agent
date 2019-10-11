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
from unittest import TestCase

from CollectorAgent.TPackets import *


# -*- coding: UTF-8 -*-
class TestPacket(TestCase):
    def test_parseNetByteStream(self):
        netFlow = struct.pack('!hii9shih',PacketType.CONTROL_HANDSHAKE,2,9,"123456789".encode(),PacketType.APPLICATION_STREAM_CLOSE,12345,0)
        view = memoryview(netFlow)

        ipacket =  Packet.parseNetByteStream(view,len(netFlow))

        tp = next(ipacket)


        self.assertEqual(tp[0], 19)
        self.assertEqual(tp[1],PacketType.CONTROL_HANDSHAKE)
        self.assertEqual(tp[2], 2)
        self.assertEqual(tp[3].tobytes(),"123456789".encode())

        tp = next(ipacket)


        self.assertEqual(tp[0], 27)
        self.assertEqual(tp[1],PacketType.APPLICATION_STREAM_CLOSE)
        self.assertEqual(tp[2],12345)
        self.assertEqual(tp[3],None)


