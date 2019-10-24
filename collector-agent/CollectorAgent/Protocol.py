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

import random
import struct

from thrift.protocol import TCompactProtocol
from thrift.transport import TTransport

from PinpointAgent.Type import SIGNATURE, HEADER_VERSION, SPAN, AGENT_INFO, AGENT_STAT, AGENT_STAT_BATCH, API_META_DATA, \
    STRING_META_DATA, RESULT, TVERSION
from Proto.Trift.Pinpoint.ttypes import TAgentInfo, TAgentStat, TAgentStatBatch
from Proto.Trift.Trace.ttypes import TSpan, TApiMetaData, TResult, TStringMetaData

class CollectorPro(object):
    # transactionId = 0
    # sequenceId = 0
    # spanid = 0
    # @staticmethod
    # def createSpanid():
    #     # return random.getrandbits(63)
    #     CollectorPro.spanid +=1
    #     return CollectorPro.spanid

    # @staticmethod
    # def createSequenceId():
    #     CollectorPro.sequenceId += 1
    #     return CollectorPro.sequenceId

    @staticmethod
    def typeHelper(type):
        '''

        :param int type:
        :return: type=> obj
        '''

        if type == SPAN:
            return TSpan()
        elif type == AGENT_INFO:
            return TAgentInfo()
        elif type == AGENT_STAT:
            return TAgentStat()
        elif type == AGENT_STAT_BATCH:
            return TAgentStatBatch()
        elif type == API_META_DATA:
            return TApiMetaData()
        elif type == STRING_META_DATA:
            return TStringMetaData()
        elif type == RESULT:
            return TResult()
        else:
            raise Exception("type:%d not found",type)

    __ReqCount = 0
    @staticmethod
    def getCurReqCount():
        CollectorPro.__ReqCount+= 1
        return CollectorPro.__ReqCount

    @staticmethod
    def obj2bin(obj, type):
        """ Serialize.
        """
        membuf = TTransport.TMemoryBuffer()
        binprot = TCompactProtocol.TCompactProtocol(membuf)

        buf = struct.pack('!BBh', SIGNATURE, HEADER_VERSION, type)
        obj.write(binprot)
        buf += membuf.getvalue()
        return buf

    # @staticmethod
    # def tspan2Bin(obj,type):


    @staticmethod
    def bin2obj(val):
        '''

        :param memoryview val:
        :return:
        '''

        (signature,version,type) = struct.unpack('!BBh',val[0:4].tobytes() )
        obj = CollectorPro.typeHelper(type)
        tmembuf = TTransport.TMemoryBuffer(val[4:].tobytes())
        tbinprot = TCompactProtocol.TCompactProtocol(tmembuf)
        obj.read(tbinprot)
        return (type,obj)

class ChannelBufferV2(object):
    def __init__(self,buf=b''):
        self.buf = buf
        self.p = 0
        self.len = len(buf)

    def putByte(self,b):
        self.buf+=struct.pack('B',b)

    # def putStr(self,str):
    #     self.buf+=struct.pack('!i',len(str))
    #     self.buf+=str

    def putI64(self,i64):
        self.buf+=struct.pack('!q',i64)

    def getBuf(self):
        return self.buf

    def readInt(self):
        assert self.len >= 4
        b,=struct.unpack('!i',self.buf[self.p:self.p+4])
        self.p += 4
        self.len -= 4
        return b

    def readStr(self,len):
        assert self.len >= len
        str = self.buf[self.p:self.p+len]
        self.p += len
        self.len -= len
        return str

    def readI64(self):
        assert self.len >= 8
        b,=struct.unpack('!q',self.buf[self.p:self.p+8])
        self.p += 8
        self.len -= 8
        return b

    def readDouble(self):
        assert self.len >= 8
        b, = struct.unpack('!d', self.buf[self.p:self.p+8])
        self.p += 8
        self.len -= 8
        return b

    def readChar(self):
        assert self.len >= 1
        b, = struct.unpack('c', self.buf[self.p:self.p+1])
        self.p += 1
        self.len -= 1
        return b

    def peekChar(self):
        assert self.len >= 1
        b, = struct.unpack('c', self.buf[self.p:self.p+1])
        return b

    def readByte(self):
        assert self.len >= 1
        b, = struct.unpack('B', self.buf[self.p:self.p+1])
        self.p += 1
        self.len -= 1
        return b

class AutoBuffer(object):
    def __init__(self):
        self.buf = b''

    def putByte(self,b):
        self.buf += struct.pack('B',b)

    def putSVar(self,sLen):
        value = (sLen <<1) ^(sLen >>31)
        self.putVar32(value)

    def putVar32(self,v):
        while True:
            if v & ~0x7F == 0:
                self.putByte(v)
                break
            else:
                self.putByte( (v & 0x7F)| 0x80)
                v = (v >> 7)

    def putPrefixedString(self,str):
        if str is None:
            self.putSVar(-1)
        else:
            self.putSVar(len(str))
            self.buf += str.encode()

    def putVar64(self,v):
        while True:
            if v & ~0x7f == 0:
                self.buf += struct.pack('B',v)
                break
            else:
                self.buf += struct.pack('B',(v & 0x7f) | 0x80)
                v = v>> 7

class TransactionId(object):
    def __init__(self,encoded_str=None,agent_id=None,start_time=None,id=None):

        if encoded_str is not None:
            agentId ,startTime ,id = encoded_str.split('^')
            self.startTime = int(startTime)
            self.id = int(id)
            self.agentId = agentId
            return

        self.agentId =agent_id
        self.startTime = start_time
        self.id=id

    def getBytes(self):
        bBuf = AutoBuffer()
        bBuf.putByte(TVERSION)
        bBuf.putPrefixedString(str(self.agentId))
        bBuf.putVar64(self.startTime)
        bBuf.putVar64(self.id)
        return bBuf.buf



if __name__ == '__main__':
    # span = TSpan()
    # test ={
    #     1:'agentId'
    # }
    # test[1]='1234'
    # print(span)
    # setattr(span,test[1],'1234')
    # print(span)

    tid = TransactionId(agent_id='test-agent',start_time=1560951035971,id=1)
    buf1 = tid.getBytes()

    tid = TransactionId(encoded_str='test-agent^1560951035971^1')
    buf2 = tid.getBytes()
    assert buf1 == buf2
    print("pass")
