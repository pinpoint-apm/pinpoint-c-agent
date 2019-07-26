#!/usr/bin/env python
# -*- coding: UTF-8 -*-


E_CONNECTED = 0
E_CONNETING = 1
E_WRITING = 2
E_READ = 4
E_CLOSED  = 8

from gevent import get_hub

class TrainLayer(object):
    # remote = (host,port)
    #
    def __init__(self,remote,msg_cb,auto_recover=True,timeout= 2):
        self.timeout     = timeout
        self.remote      = remote
        self.in_msg_cb     = msg_cb
        self.state = E_CLOSED

    def start(self):
        pass

    def stop(self):
        pass

    @staticmethod
    def registerTimers(callback,interval,args=None):
        loop = get_hub().loop
        time = loop.timer(0, interval)
        time.start(callback,args)


if __name__ == '__main__':
    # print(gsocket.getaddrinfo("www.google.jp", 80))
    pass
