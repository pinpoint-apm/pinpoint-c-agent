#!/usr/bin/env python
from unittest import TestCase

from TPackets import *


# -*- coding: UTF-8 -*-
class TestPacket(TestCase):
    def test_parseNetByteStream(self):
        netFlow = struct.pack('!hii9shih',PacketType.CONTROL_HANDSHAKE,2,9,"123456789",PacketType.APPLICATION_STREAM_CLOSE,12345,0)
        view = memoryview(netFlow)

        ipacket =  Packet.parseNetByteStream(view,len(netFlow))

        tp = ipacket.next()


        self.assertEqual(tp[0], 19)
        self.assertEqual(tp[1],PacketType.CONTROL_HANDSHAKE)
        self.assertEqual(tp[2], 2)
        self.assertEqual(tp[3].tobytes(),"123456789")

        tp = ipacket.next()


        self.assertEqual(tp[0], 27)
        self.assertEqual(tp[1],PacketType.APPLICATION_STREAM_CLOSE)
        self.assertEqual(tp[2],12345)
        self.assertEqual(tp[3],None)


