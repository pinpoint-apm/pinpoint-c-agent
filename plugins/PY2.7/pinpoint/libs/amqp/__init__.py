#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 12/3/20

def monkey_patch():
    from pinpoint.common import Interceptor
    try:
        import amqp
        from .AmqpPlugin import AmqpPlugin

        Interceptors = [
            Interceptor(amqp.Channel,'basic_publish_confirm', AmqpPlugin)
        ]

        for interceptor in Interceptors:
            interceptor.enable()

    except ImportError as e:
        print(e)

__all__=['monkey_patch']