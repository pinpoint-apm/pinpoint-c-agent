#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 8/20/20

class Interceptor:
    def __init__(self, scope, point, handle):
        assert isinstance(point,str)
        self.full_name = scope.__module__+'.'+point
        self.point = point
        # new a handle and bind on full_name
        self.handle = handle(self.full_name)
        self.scope = scope
        self.origin_point = getattr(scope,point)

    def enable(self):
        assert callable(self.handle)
        setattr(self.scope, self.point, self.handle(self.origin_point))

    def disable(self):
        setattr(self.scope, self.point, self.origin_point)