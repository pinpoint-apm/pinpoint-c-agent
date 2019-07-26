#!/usr/bin/env python
# -*- coding: UTF-8 -*-


from Common import *
from Protocol import *




class Packet(object):
    
    TYPE_LENGTH = 2
    def __init__(self, hType, hTypeEx1 = None, hTypeEx2= None, body =None):
        self.buf = struct.pack( "!h",hType)

        if hTypeEx1 is not None:
            self.buf += struct.pack('!i',hTypeEx1)

        if hTypeEx2 is not None:
            self.buf += struct.pack('!i', hTypeEx2)

        if body is not None:
            self.buf += body

        TCLogger.debug("current type:%d  totalLen:%d", hType, len(self.buf))

    def getSerializedData(self):
        return self.buf



    @staticmethod
    def parseNetByteStream(view,size):
        '''
        size must > 2
        return type and packet
        :param  memoryview view:
        :param int size:
        :return:
        '''

        if size < 2:
            return

        offset = 0
        while offset < size:
            ### parse type
            type, = struct.unpack("!h",view[offset:offset+Packet.TYPE_LENGTH])
            if type  not in PacketType.PTypeBytesLayOut:
                raise Exception("type:%d not found",type)

            ### parse type ->sub header
            body_header_len,sub_header_len,sub_body_len_offset = PacketType.PTypeBytesLayOut[type]
            if size - Packet.TYPE_LENGTH< body_header_len: ## not enough
                # yield (Packet.READ_AGAIN,size - offset)
                return ## stop iteration
            offset += 2
            header = 0
            ### get sub header
            if sub_header_len == 4:
                header, = struct.unpack("!i",view[offset:offset+sub_header_len])
                offset+= sub_header_len

            ### parse body_length
            body_len = 0
            if sub_body_len_offset == 2:
                body_len, =  struct.unpack("!h",view[offset:offset + sub_body_len_offset])
            elif sub_body_len_offset == 4:
                body_len, =  struct.unpack("!i", view[offset:offset + sub_body_len_offset])
            elif sub_body_len_offset == 0:
                yield (offset + sub_body_len_offset,type,header,None)
                continue ## goto parse type

            offset += sub_body_len_offset
            if size - offset < body_len: ## not enough
                # yield (Packet.READ_AGAIN, size - body_header_len - Packet.TYPE_LENGTH)
                return  ## stop iteration
            else:
                if body_len>0:
                    yield (offset+body_len ,type,header,view[offset:offset+body_len])
                else:
                    yield (offset , type, header, None)
                offset += body_len


            


class HandShakeMessage:
    handShakeCount = 0


    def __init__(self,socketId_i,hostName_s,supportServer_b,ip_s,agentId_s,
                 applicationName_s,serviceType_i,pid_i,version_s,startTimestamp_i64):
        cMbuf = ControlMessageEncoder()

        cMbuf.writeString('socketId')
        cMbuf.writeInt(socketId_i)

        cMbuf.writeString('hostName')
        cMbuf.writeString(hostName_s)

        cMbuf.writeString('supportServer')
        cMbuf.writeBool(supportServer_b)

        cMbuf.writeString('ip')
        cMbuf.writeString(ip_s)

        cMbuf.writeString('agentId')
        cMbuf.writeString(agentId_s)

        cMbuf.writeString('applicationName')
        cMbuf.writeString(applicationName_s)

        cMbuf.writeString('serviceType')
        cMbuf.writeInt(serviceType_i)

        cMbuf.writeString('pid')
        cMbuf.writeInt(pid_i)

        cMbuf.writeString('version')
        cMbuf.writeString(version_s)

        cMbuf.writeString('startTimestamp')
        cMbuf.writeInt64(startTimestamp_i64)
        cMbuf.endEncoder()

        HandShakeMessage.handShakeCount+= 1

        self.chBuf = cMbuf

    @staticmethod
    def getCount():
        return HandShakeMessage.handShakeCount

    @staticmethod
    def getNextStateCode(code,subCode):
        SUCCESS = (0, 0, "Success.")
        SIMPLEX_COMMUNICATION = (0, 1, "Simplex Connection successfully established.")
        DUPLEX_COMMUNICATION = (0, 2, "Duplex Connection successfully established.")
        ALREADY_SIMPLEX_COMMUNICATION = (1, 1, "Already Simplex Connection established.")
        ALREADY_DUPLEX_COMMUNICATION = (1, 2, "Already Duplex Connection established.")
        # ALREADY_KNOWN = (1, 0, "Already Known.")
        # PROPERTY_ERROR = (2, 0, "Property error.")
        # PROTOCOL_ERROR = (3, 0, "Illegal protocol error.")
        # UNKNOWN_ERROR = (4, 0, "Unknown Error.")
        # UNKNOWN_CODE = (-1, -1, "Unknown Code.")
        states =(
            (0, 0, "Success.",AgentSocketCode.RUN_SIMPLEX),
            (0, 1, "Simplex Connection successfully established.",AgentSocketCode.RUN_SIMPLEX),
            (0, 2, "Duplex Connection successfully established.",AgentSocketCode.RUN_DUPLEX),
            (1, 1, "Already Simplex Connection established.",AgentSocketCode.RUN_SIMPLEX),
            (1, 2, "Already Duplex Connection established.",AgentSocketCode.RUN_DUPLEX)
        )

        for s in states:
            if s[0] == code and s[1] == subCode:
                return s[3]
        raise Exception("state not register")


    def getDataLen(self):
        return len(self.chBuf.getRawData())

    def getBinData(self):
        return self.chBuf.getRawData()

class ControlMessage(object):
    CONTROL_MESSAGE_UNKNOWN = -1
    CONTROL_MESSAGE_NULL = 0
    CONTROL_MESSAGE_BOOL = 1
    CONTROL_MESSAGE_LONG = 2
    CONTROL_MESSAGE_DOUBLE = 3
    CONTROL_MESSAGE_STRING = 4
    CONTROL_MESSAGE_LIST = 5
    CONTROL_MESSAGE_MAP = 6
    def __init__(self):
        self.type = ControlMessage.CONTROL_MESSAGE_UNKNOWN
        self.data = None


class ControlMessageDecoder(object):
    @staticmethod
    def __readStringLength(cbv):
        result = 0
        shift = 0
        while True:
            b = cbv.readByte()
            result |=(b & 0x7F) << shift
            if b& 0x80 != 128:
                break
            shift += 7
        return result

    @staticmethod
    def __decodeString(cbv):
        length = ControlMessageDecoder.__readStringLength(cbv)

        return cbv.readStr(length)

    @staticmethod
    def decodingControlMessage(cbv):
        '''

        :param ChannelBufferV2 cbv:
        :return:
        '''
        cmg = ControlMessage()
        type = cbv.readChar()
        if type == ControlMessageProtocolConstant.TYPE_CHARACTER_BOOL_TRUE:
            cmg.type = ControlMessage.CONTROL_MESSAGE_BOOL
            cmg.data = True
        elif type == ControlMessageProtocolConstant.TYPE_CHARACTER_NULL:
            cmg.type = ControlMessage.CONTROL_MESSAGE_NULL
        elif type == ControlMessageProtocolConstant.TYPE_CHARACTER_BOOL_FALSE:
            cmg.type = ControlMessage.CONTROL_MESSAGE_BOOL
            cmg.data = False
        elif type == ControlMessageProtocolConstant.TYPE_CHARACTER_INT:
            cmg.type = ControlMessage.CONTROL_MESSAGE_LONG
            cmg.data = cbv.readInt()
        elif type == ControlMessageProtocolConstant.TYPE_CHARACTER_LONG:
            cmg.type = ControlMessage.CONTROL_MESSAGE_LONG
            cmg.data = cbv.readI64()
        elif type == ControlMessageProtocolConstant.TYPE_CHARACTER_DOUBLE:
            cmg.type = ControlMessage.CONTROL_MESSAGE_DOUBLE
            cmg.data = cbv.readDouble()
        elif type == ControlMessageProtocolConstant.TYPE_CHARACTER_STRING:
            cmg.type = ControlMessage.CONTROL_MESSAGE_STRING
            cmg.data = ControlMessageDecoder.__decodeString(cbv)
        elif type == ControlMessageProtocolConstant.CONTROL_CHARACTER_LIST_START:
            cmg.type = ControlMessage.CONTROL_MESSAGE_LIST
            cmg.data = []

            while cbv.peekChar() != ControlMessageProtocolConstant.CONTROL_CHARACTER_LIST_END:
                cmg.data.append(ControlMessageDecoder.decodingControlMessage(cbv))
            cbv.readByte() ## drop the EOF
        elif type == ControlMessageProtocolConstant.CONTROL_CHARACTER_MAP_START:
            cmg.type = ControlMessage.CONTROL_MESSAGE_MAP
            cmg.data = {}
            while cbv.peekChar() != ControlMessageProtocolConstant.CONTROL_CHARACTER_MAP_END:
                key = ControlMessageDecoder.decodingControlMessage(cbv)
                value = ControlMessageDecoder.decodingControlMessage(cbv)
                cmg.data[key.data] = value
            cbv.readByte()

        return cmg



class ControlMessageEncoder(object):
    def __init__(self):
        self.tmp_buf = struct.pack('c', ControlMessageProtocolConstant.CONTROL_CHARACTER_MAP_START)
        self.__buf = ''

    def writeChar(self, b):
        self.tmp_buf += struct.pack("c", b)

    def writeByte(self,i8):
        self.tmp_buf += struct.pack("b", i8)

    def writeInt(self,i):
        self.tmp_buf += struct.pack("!ci", ControlMessageProtocolConstant.TYPE_CHARACTER_INT, i)

    def writeInt64(self, l64):
        self.tmp_buf += struct.pack("!cq", ControlMessageProtocolConstant.TYPE_CHARACTER_LONG, l64)

    def writeString(self, string):
        self.tmp_buf += struct.pack("!cb", ControlMessageProtocolConstant.TYPE_CHARACTER_STRING, len(string))
        self.tmp_buf += string.encode()

    def writeBool(self, flag):
        if flag:
            self.tmp_buf += struct.pack("c", ControlMessageProtocolConstant.TYPE_CHARACTER_BOOL_TRUE)
        else:
            self.tmp_buf += struct.pack("c", ControlMessageProtocolConstant.TYPE_CHARACTER_BOOL_FALSE)

    def endEncoder(self):
        self.tmp_buf += struct.pack('c', ControlMessageProtocolConstant.CONTROL_CHARACTER_MAP_END)
        self.__buf = self.tmp_buf[:]

    def getRawData(self):
        return self.__buf


def test_yeild(buf,size):
    i= 0
    while i<size:
        yield buf[i]
        i+=1
        print(i)
        if i >=1:
            break




if __name__ == '__main__':
    # s = struct.pack("2i13si6s2i", 33, 13, "www.baidu.com", 6, "¶¬¼¾", 0, 0)
    # p = Packet(1,1234,len(s),s)
    # print(p.getSerializedData())
    #
    # buf = ChannelBufferV2()
    # buf.writeString("hello")
    # f1 = test_yeild("abcd3",5)
    #
    # print( f1.next())
    # print( f1.next())

    a = bytearray(['1','2','3',"0"])
    pa = memoryview(a)
    pa[2:4] = pa[0:2]
    print(a)

    print(struct.pack('!hii',12,None,None))
