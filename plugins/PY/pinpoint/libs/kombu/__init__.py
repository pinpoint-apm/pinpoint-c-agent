#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 12/29/20

from pinpoint.common import Interceptor


def monkey_patch():
    try:
        from kombu.messaging import Producer
        from .ProducerPlugin import ProducerPlugin

        Interceptors = [
            Interceptor(Producer, 'publish', ProducerPlugin),
        ]
        for interceptor in Interceptors:
            interceptor.enable()


    except ImportError as e:
        # do nothing
        print(e)

__all__=['monkey_patch']