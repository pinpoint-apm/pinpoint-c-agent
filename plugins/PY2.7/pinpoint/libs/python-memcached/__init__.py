#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 12/3/20

def monkey_patch():
    from pinpoint.common import Interceptor
    try:
        from memcache import Client
        from .PyMemcachedPlugin import PyMemcachedPlugin
        Interceptors = [
            Interceptor(Client,'_get', PyMemcachedPlugin)
        ]

        for interceptor in Interceptors:
            interceptor.enable()

    except ImportError as e:
        print(e)

__all__=['monkey_patch']