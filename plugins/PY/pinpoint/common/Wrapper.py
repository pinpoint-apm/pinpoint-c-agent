#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 11/11/20

class Wrapper:
    def __init__(self,obj):
        self.origObj = obj

    def __getattr__(self, item):
        print(item)
        if item in self.__dict__:
            return getattr(self,item)
        else:
            return getattr(self.origObj, item)
