#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 1/14/21
from Stat_pb2 import PJvmGc, JVM_GC_TYPE_CMS


class Stat:
    @staticmethod
    def collectJvmInfo():
        # php/python no such jvm info
        jvmGc = PJvmGc(type=JVM_GC_TYPE_CMS
                       )
        return jvmGc
