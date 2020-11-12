#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 8/20/20

try:
    import redis
    from .PyRedisPlugins import PyRedisPlugins
    HookSet = [
        ('Redis', 'Redis', redis, redis.Redis),
        ('Redis','execute_command', redis, redis.Redis.execute_command),
    ]

    for hook in HookSet:
        new_Cursor = PyRedisPlugins(hook[0], '')
        setattr(hook[2],hook[1], new_Cursor(hook[3]))
except ImportError as e:
    # do nothing
    print(e)