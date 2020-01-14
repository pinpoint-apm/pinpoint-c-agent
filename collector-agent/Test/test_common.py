#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 10/16/19

# from queue import  Queue
# import threading,time
#
# queue = Queue()
# def product():
#     obj =100000
#     while obj>0:
#         queue.put(obj)
#         print("pro: queue size:%d"%(queue.qsize()))
#         obj-=1
#         time.sleep(3)
#
# def consumer():
#     while True:
#         obj = queue.get()
#         print("con: %d"%obj)
#
# def consumer1():
#     while True:
#         obj = queue.get()
#         print("con: %d"%obj)
#
# if __name__ == '__main__':
#
#     pro = threading.Thread(target=product,args=())
#     con = threading.Thread(target=consumer, args=())
#     con1 = threading.Thread(target=consumer, args=())
#     con2 = threading.Thread(target=consumer1, args=())
#
#     pro.start()
#     con.start()
#     con2.start()
#     con1.start()
#     pro.join()
#     con.join()
#     con2.join()
#     con1.join()


# from multiprocessing import Process, Queue
# import os

# def run_in_process(queue):
#     while True:
#         obj = queue.get()
#         print("%s %d \n"%(obj,os.getpid()))

# if __name__ == '__main__':
#     queue = Queue()

#     pro1 = Process(target=run_in_process,args=(queue,))
#     pro2 = Process(target=run_in_process,args=(queue,))
#     pro1.start()
#     pro2.start()
#     while True:
#         queue.put('a')


import zmq
import random
import time
import os
from multiprocessing import Process

def push_main():
    context = zmq.Context()

    # Socket to send messages on
    sender = context.socket(zmq.PUSH)
    sender.bind("tcp://*:5557")
    i = 0
    while True:
        sender.send(b'hello: %d'%(i))
        print("send %d\n"%(i))
        i+=1

def pull_main():
    context = zmq.Context()

    # Socket to receive messages on
    receiver = context.socket(zmq.PULL)
    receiver.connect("tcp://localhost:5557")

    # Process tasks forever
    while True:
        s = receiver.recv()
        # time.sleep(1)
        print("recv %d %s\n"%(os.getpid(),s))
    
if __name__ == '__main__':
    push_process = Process(target=push_main)
    pull_process1 = Process(target=pull_main)
    pull_process2 = Process(target=pull_main)

    push_process.start()
    pull_process1.start()
    pull_process2.start()
    push_process.join()
    pull_process1.join()
    pull_process2.join()




