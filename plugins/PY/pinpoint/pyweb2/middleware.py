#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 11/12/20

from .RequestPlugin import RequestPlugin

from pinpoint.libs import monkey_patch_for_pinpoint
monkey_patch_for_pinpoint()

def PinPointMiddleWare(application):
    def handler(environ,start_response):
        plugin = RequestPlugin("pyweb-request")
        plugin.onBefore(environ,start_response)
        response = application(environ,start_response)
        plugin.onEnd(response)
        return response
    return handler
