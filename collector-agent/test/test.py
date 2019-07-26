#!/usr/bin/env python
#coding:utf-8


# from tornado.ioloop import IOLoop
# from tornado import gen
# from tornado.tcpclient import TCPClient
# from tornado.options import options, define
# from Type  import PacketType
#
# define("host", default="localhost", help="TCP server host")
# define("port", default=9888, help="TCP port to connect to")
# define("message", default="ping", help="Message to send")
#
#
# @gen.coroutine
# def send_message():
#     stream = yield TCPClient().connect(options.host, options.port)
#     yield stream.write((options.message + "\n").encode())
#     print("Sent to server:", options.message)
#     reply = yield stream.read_until(b"\n")
#     print("Response from server:", reply.decode().strip())
#
#
# if __name__ == "__main__":
#     options.parse_command_line()
#     IOLoop.current().run_sync(send_message)


# rom
# tornado
# import ioloop
# import datetime
#
#
# # class MainHandler(web.RequestHandler):
# #     def get(self):
# #         self.write('Hello Tornado')
#
#
# def f2s():
#     print '2s ', datetime.datetime.now()
#
#
# def f5s():
#     print '5s ', datetime.datetime.now()
#
#
# if __name__ == '__main__':
#     # application = web.Application([
#     #     (r'/', MainHandler),
#     # ])
#     # application.listen(8081)
#     ioloop.PeriodicCallback(f2s, 2000).start()  # start scheduler 每隔2s执行一次f2s
#     ioloop.PeriodicCallback(f5s, 5000).start()  # start scheduler
#     ioloop.IOLoop.instance().start()
# f



# class Test(object):
#     A= 10
#     def __init__(self):
#         Test.A += 1
#         print "rftgyhjo;'", Test.A
#         self.call =[self.test_cb]
#
#     def test_cb(self, p):
#         print p
#
#     def callF(self,p):
#         self.call[0](p)
# a = Test()
# b = Test()
# b.callF('fghjk')

# import gevent
# from gevent.pool import Group
# import  gevent.time as time
# #
# def talk(msg):
#     while True:
#         print(msg)
#         time.sleep(1)
#
#
# g1 = gevent.spawn(talk, 'bar')
# g2 = gevent.spawn(talk, 'foo')
# g3 = gevent.spawn(talk, 'fizz')
#
# group = Group()
# group.add(g1)
# group.add(g2)
# g1.kill()
# g2.kill()
# g3.kill()
# group.join()

# group.add(g3)
# group.join()
#
# def stop():
#     g1.kill()
#     g2.kill()
#
# g4 = gevent.spawn(stop)
# group.add(g4)
# group.join()



# class foo(object):
#     def __init__(self):
#         self.a =1
#         self.b= 3
#     def __str__(self):
#         return "a:%d b:%d"%(self.a,self.b)
#
#     def gogo(self):
#         self.a +=1
#         self.b +=1
#         a = self
#         return a
#
# f = foo()
# a=  f.gogo()
# print a

#
# from gevent.event import Event
# import gevent
# import signal
# evt = Event()
#
# gevent.signal(signal.SIGQUIT, evt.set)
# gevent.signal(signal.SIGTERM, evt.set)
# gevent.signal(signal.SIGINT, evt.set)
#
# while True:
#     gevent.sleep(1)
#     if evt.wait(0):
#         break
#
#
#
# print("catch event")


# # write a simple example file
with open("hello.txt", "wb") as f:
    # f.write(b"Hello Python!\n")
    f.truncate(1024)
    f.close()
#
# pid = os.fork()
# if pid == 0:
#     with open("hello.txt", "r+b") as f:
#         mm = mmap.mmap(f.fileno(),0)
#         while True:
#             print(mm[:5])
#             time.sleep(1)
#
#
#
# with open("hello.txt", "r+b") as f:
#     # memory-map the file, size 0 means whole file
#     mm = mmap.mmap(f.fileno(), 0)
#     sequence = 10000
#     while True:
#
#         mm[:5]= str(sequence).encode()
#         sequence+=1
#         print("pid:%d seq:%d",os.getpid(),sequence)
#         time.sleep(1)


# import configparser as ConfigParser
# import itertools
#
# config = ConfigParser.ConfigParser()
#
# filename = "/home/test/var/php7/lib/php.ini"
# with open(filename) as fp:
#     config.read_file(itertools.chain(['[global]'],fp),source=filename)
# # config.read_file()
#
# print(config.get("pinpoint","pinpoint_php.CollectorHost"))