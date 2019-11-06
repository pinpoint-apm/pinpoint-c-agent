import struct
import time

from gevent import Timeout
from gevent import socket as gsocket


def output_src(socket,address):
    print('New connection from %s' % address)
    while True:
        header = socket.recv(4)
        len, = struct.unpack('!i', header)
        print(len)
        body = socket.recv(len)
        print(body)


def timer_send_cc(socket):
    socket.send("ccc")

def timer_send(socket):
    socket.send("ttt")

count = 0

def client():
    global count
    count+=1
    s = gsocket.socket(gsocket.AF_UNIX, gsocket.SOCK_STREAM)

    try:
        s.connect(('/tmp/collector-agent.sock'))
    except Timeout as t:
        print("connect time out: %s"%str(t))
    except Exception as e:
        print(str(e))
    json='{"a":1,"c":"123456789"}'
    import struct
    data = struct.pack('!ii',1,0)
    s.send(data)
    data =s.recv(1024)
    s.close()


if __name__ == '__main__':
    time_s = time.time()
    i = 10000
    while i > 0:
        client()
        i-=1
    print(" speed:%f"%(time.time() - time_s))