#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from gevent.server import StreamServer

def handle(socket, address):
    print('new connection!')

    while 1:
        data = socket.recv(1024)
        if len(data) == 0:
            print("%s connection close"%(address[0]))

            break


server = StreamServer(('0.0.0.0', 1234), handle)  # creates a new server
server.serve_forever()