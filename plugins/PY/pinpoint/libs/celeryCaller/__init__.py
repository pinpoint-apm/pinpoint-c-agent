#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 12/29/20

from pinpoint.common import Interceptor


def monkey_patch():
    try:
        import celery
        from celery.app.task import Task
        from celery.result import AsyncResult

        from .TaskPlugin import TaskPlugin
        from .GetPlugin import GetPlugin

        Interceptors = [
            Interceptor(Task, 'delay', TaskPlugin),
            Interceptor(AsyncResult, 'get', GetPlugin),
        ]
        for interceptor in Interceptors:
            interceptor.enable()
        print('-------------------------------------------')
    except ImportError as e:
        # do nothing
        print(e)

__all__=['monkey_patch']