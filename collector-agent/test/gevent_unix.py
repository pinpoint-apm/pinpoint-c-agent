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

    # s.settimeout(2)
    try:
        s.connect(('/tmp/collector-agent.sock'))
    except Timeout as t:
        print("connect time out: %s"%str(t))
    except Exception as e:
        print(str(e))
    json='{"a":1,"c":"123456789"}'
    import struct
    data = struct.pack('!ii',1,0)
    # data+= json
    # json = '{"a":1,"c":"abcdefegttes"}'
    # data+= struct.pack('!i', len(json))
    # data+= json
    s.send(data)
    data =s.recv(1024)
    # loop = gevent.get_hub().loop
    # time = loop.timer(1, 1)
    # time.start(timer_send,s)
    # time = loop.timer(1, 1)
    # time.start(timer_send_cc,s)
    # while True:
    #     data =   s.recv(1024)
    #     print data

    s.close()


if __name__ == '__main__':
    # listener = gsocket.socket(gsocket.AF_UNIX, gsocket.SOCK_STREAM)
    # sockname = '/tmp/collector-agent.sock';
    # if os.path.exists(sockname):
    #     os.remove(sockname)
    # listener.bind(sockname)
    # listener.listen(256)

    time_s = time.time()
    i = 10000
    while i > 0:
        client()
        i-=1

    print(" speed:%f",(time.time() - time_s)/10000 )
    # server = StreamServer(listener,output_src)
    # server.start()
    # server.serve_forever()
    # gevent.spawn(client).join();
    # gevent.joinall()
    # client()