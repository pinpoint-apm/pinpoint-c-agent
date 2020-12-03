#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 12/3/20

def monkey_patch():
    from pinpoint.common import Interceptor
    try:
        from pymemcache.client.base import Client
        from .PymemcachePlugin import PymemcachePlugin
        Interceptors = [
            Interceptor(Client,'_fetch_cmd', PymemcachePlugin)
        ]

        for interceptor in Interceptors:
            interceptor.enable()

    except ImportError as e:
        print(e)

__all__=['monkey_patch']