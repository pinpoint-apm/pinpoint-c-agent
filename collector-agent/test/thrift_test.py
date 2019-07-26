from thrift.protocol import *
from thrift.transport import TTransport

from Trace.ttypes import TSpan


def serialize(th_obj):
    """ Serialize.
    """
    tmembuf = TTransport.TMemoryBuffer()
    # tbinprot = TBinaryProtocol.TBinaryProtocol(tmembuf)
    tCombuf = TCompactProtocol.TCompactProtocol(tmembuf)
    tCombuf.writeByte()


    th_obj.write(tCombuf)
    return tmembuf.getvalue()

def deserialize(val, th_obj_type):
    """ Deserialize.
    """
    th_obj = th_obj_type()
    tmembuf = TTransport.TMemoryBuffer(val)
    tbinprot = TBinaryProtocol.TBinaryProtocol(tmembuf)
    th_obj.read(tbinprot)
    return th_obj

# print sys.path
node = TSpan()
node.apiId=123
node.acceptorHost="23423"

val = serialize(node)
print len(val)
va2 = deserialize(val,TSpan)

print va2.apiId,va2.acceptorHost

