#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 11/7/19

import os
import sys

proto_class = os.getcwd() + '/Proto/grpc'
sys.path.append(proto_class)

import unittest
from unittest import TestCase

import gevent

from CollectorAgent.CollectorAgentConf import CollectorAgentConf
from PinpointAgent.AppManagement import AppManagement
import os


@unittest.skip('thrift not fully test')
class TestImplement(TestCase):
    def setUp(self) -> None:
        path = os.getcwd()
        self.grpc_config = path+'/conf/collector_grpc.conf'
        self.thrift_config = path+'/conf/collector_thrift.conf'

    def test_thrift(self):
        os.environ['COLLECTOR_CONFIG'] = self.thrift_config
        self.run_app_manager()

    def test_grpc(self):
        os.environ['COLLECTOR_CONFIG'] = self.grpc_config
        self.run_app_manager()

    def run_app_manager(self):
        from Common.Config import CAConfig
        collector_conf = CollectorAgentConf(CAConfig)
        app_management = AppManagement(collector_conf)
        i =10000
        while i >0:
            # stime =1573438683
            # span = '{"name":"PHP Request","FT":1500,' \
            #        '"server":"10.34.130.79:28081",' \
            #        '"sid":"3345567788","psid":"3345567789","tid":"phpmyadmin^1560951035971^1",' \
            #        '"S":1573438683,"E":20,' \
            #        '"clues":["46:200"],' \
            #        '"uri":"/index.html",' \
            #        '"ERR":{"msg":"23456789op[xdcfvgbhnjmk"},' \
            #        '"EC":1, "estr":"DIY",' \
            #        '"calls":[{"name":"hello","S":1573438683,"E":8,"calls":[{"name":"hello2","1573438683":2,"E":2,"clues":["-1:null","14:2019/06/25"],"calls":[{"name":"hello3","S":1573438683,"E":4}]}]}],' \
            #        '"client":"10.10.10.10"}'
            span ='''{"AP":"t=10 D=10 i=10 b=10","E":1,"FT":1500,"S":1573549175197,"appid":"app-2","appname":"APP-2","calls":[{"E":1,"S":0,"calls":[{"E":0,"S":1,"clues":["-1:Array()","14:ture"],"name":"date","stp":"1501"}],"clues":["-1:Array","14:ture"],"name":"AppDate::outputDate","stp":"1501"}],"client":"10.34.130.156","clues":["46:200"],"name":"PHP Request","server":"10.34.130.134:8001","sid":"213111260","stp":"1500","tid":"app-2^1573547333369^13794","uri":"/index.php?type=get_date"}'''
            app_management.handleFrontAgentData(None, None, span.encode())
            i-= 1
            # time.sleep(3)
            # gevent.sleep(1)
        while True:
            gevent.sleep(1)
# GRPC_TRACE=all;GRPC_VERBOSITY=DEBUG