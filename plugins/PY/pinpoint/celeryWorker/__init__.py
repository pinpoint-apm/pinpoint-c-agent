#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 1/7/21

from pinpoint.common import Interceptor


from pinpoint.libs import monkey_patch_for_pinpoint
monkey_patch_for_pinpoint(amqp=False,kombu=False)

def monkey_patch():
    try:
        from .WorkerPlugin import WorkerPlugin
        from celery.app import trace
        Interceptors = [
            Interceptor(trace, 'trace_task', WorkerPlugin),
        ]
        for interceptor in Interceptors:
            interceptor.enable()

    except ImportError as e:
         # do nothing
        print(e)


__all__ = ['monkey_patch']