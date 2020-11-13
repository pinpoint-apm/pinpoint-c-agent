#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 8/20/20

def monkey_patch():
    from pinpoint.common import Interceptor
    from .PyRedisPlugins import PyRedisPlugins
    try:
        import redis
        Interceptors = [
            Interceptor(redis.Redis, 'Redis', PyRedisPlugins),
            Interceptor(redis.Redis.execute_command, 'execute_command', PyRedisPlugins)
        ]
        for interceptor in Interceptors:
            interceptor.enable()

    except ImportError as e:
        # do nothing
        print(e)

__all__=['monkey_patch']